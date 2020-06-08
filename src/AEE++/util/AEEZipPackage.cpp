
#include "zmaee_stdlib.h"
#include "zmaee_helper.h"
#include "AEEZipPackage.h"
#include "AEEBase.h"


/*=========================================================================
 * JAR Data Stream structure
 *=======================================================================*/

#define LOCSIG (('P' << 0) + ('K' << 8) + (3 << 16) + (4 << 24))
#define CENSIG (('P' << 0) + ('K' << 8) + (1 << 16) + (2 << 24))
#define ENDSIG (('P' << 0) + ('K' << 8) + (5 << 16) + (6 << 24))

/*
 * Supported compression types
 */
#define STORED      0
#define DEFLATED    8

/*
 * Header sizes including signatures
 */
#define LOCHDRSIZ 30
#define CENHDRSIZ 46
#define ENDHDRSIZ 22

/*
 * Header field access macros
 */
#define CH(b, n) ((long)(((unsigned char *)(b))[n]))
#define SH(b, n) ((long)(CH(b, n) | (CH(b, n+1) << 8)))
#define LG(b, n) ((long)(SH(b, n) | (SH(b, n+2) << 16)))

#define GETSIG(b) LG(b, 0)      /* signature */

/*
 * Macros for getting local file header (LOC) fields
 */
#define LOCVER(b) SH(b, 4)      /* version needed to extract */
#define LOCFLG(b) SH(b, 6)      /* encrypt flags */
#define LOCHOW(b) SH(b, 8)      /* compression method */
#define LOCTIM(b) LG(b, 10)     /* modification time */
#define LOCCRC(b) LG(b, 14)     /* uncompressed file crc-32 value */
#define LOCSIZ(b) LG(b, 18)     /* compressed size */
#define LOCLEN(b) LG(b, 22)     /* uncompressed size */
#define LOCNAM(b) SH(b, 26)     /* filename size */
#define LOCEXT(b) SH(b, 28)     /* extra field size */

/*
 * Macros for getting central directory header (CEN) fields
 */
#define CENVEM(b) SH(b, 4)      /* version made by */
#define CENVER(b) SH(b, 6)      /* version needed to extract */
#define CENFLG(b) SH(b, 8)      /* general purpose bit flags */
#define CENHOW(b) SH(b, 10)     /* compression method */
#define CENTIM(b) LG(b, 12)     /* file modification time (DOS format) */
#define CENCRC(b) LG(b, 16)     /* crc of uncompressed data */
#define CENSIZ(b) LG(b, 20)     /* compressed size */
#define CENLEN(b) LG(b, 24)     /* uncompressed size */
#define CENNAM(b) SH(b, 28)     /* length of filename */
#define CENEXT(b) SH(b, 30)     /* length of extra field */
#define CENCOM(b) SH(b, 32)     /* file comment length */
#define CENDSK(b) SH(b, 34)     /* disk number start */
#define CENATT(b) SH(b, 36)     /* internal file attributes */
#define CENATX(b) LG(b, 38)     /* external file attributes */
#define CENOFF(b) LG(b, 42)     /* offset of local header */

/*
 * Macros for getting end of central directory header (END) fields
 */
#define ENDSUB(b) SH(b, 8)      /* number of entries on this disk */
#define ENDTOT(b) SH(b, 10)     /* total number of entries */
#define ENDSIZ(b) LG(b, 12)     /* central directory size */
#define ENDOFF(b) LG(b, 16)     /* central directory offset */
#define ENDCOM(b) SH(b, 20)     /* size of zip file comment */


AEEZipPackage::~AEEZipPackage()
{
	Release();
}

int AEEZipPackage::GetPackageItem(int index, unsigned char* data, int nMaxSize)
{
	int size;
	if (index < 0 || index >= mItemCount)
		return -1;
	size = GetItemSize(index);
	if (size < 0)
		return -1;
	if(nMaxSize > 0 && nMaxSize < size)
		return -1;
	if (size == 0)
		return 0;
	if (AEE_IFile_Seek(mFile, ZM_SEEK_START, mItemOffset[index]) < 0 ||
		AEE_IFile_Read(mFile, data, size) != size) {
		return -1;
	}
	return 0;
}

int AEEZipPackage::GetIndex(const char* name) const
{
	int i;
	for (i = mItemCount - 1; i >= 0; --i) {
		if (zmaee_strcmp(name, mItemName[i]) == 0)
			return i;
	}
	return -1;
}

void AEEZipPackage::Release()
{
	mLoadSucc = false;
	if (mFile) {
		AEE_IFile_Release(mFile);
		mFile = NULL;
	}
	if (mItemOffset) {
		AEEAlloc::Free(mItemOffset);
		mItemOffset = NULL;
	}
	if (mItemSize) {
		AEEAlloc::Free(mItemSize);
		mItemSize = NULL;
	}
	if (mItemName) {
		int i;
		for (i = mItemCount - 1; i >= 0; --i) {
			if (mItemName[i]) {
				AEEAlloc::Free(mItemName[i]);
			}
		}
		AEEAlloc::Free(mItemName);
		mItemName = NULL;
	}
}

AEEZipPackage::AEEZipPackage(const char* pathname)
{
	AEE_IShell* shell = ZMAEE_GetShell();
	AEE_IFileMgr* filemgr;
	AEE_IFile* file;
    int offset; /* offset of first header */
	int i;
	long currentOffset, minOffset;
    long length;

	unsigned const char *bp;
	
	unsigned char buffer[256];

	int bufferSize = sizeof(buffer);
	unsigned long locOffset;
	unsigned long cenOffset;

	mFile = NULL;
	mItemName = NULL;
	mItemOffset = NULL;
	mItemSize = NULL;

	AEE_IShell_CreateInstance(shell, ZM_AEE_CLSID_FILEMGR, (void**)&filemgr);
	mFile = file = AEE_IFileMgr_OpenFile(filemgr, pathname, ZM_OFM_READONLY);
	AEE_IFileMgr_Release(filemgr);
	if (file == NULL)
		goto loadFailed;

	mLoadSucc = true ;

	AEE_IFile_Seek(file, ZM_SEEK_END, 0);
    length = AEE_IFile_Tell(file);
    AEE_IFile_Seek(file, ZM_SEEK_START, 0);

    minOffset = length - (0xFFFF + ENDHDRSIZ);
    if (minOffset < LOCHDRSIZ + CENHDRSIZ) {
        minOffset = LOCHDRSIZ + CENHDRSIZ;
    }

    if ((AEE_IFile_Seek(file, ZM_SEEK_END, -ENDHDRSIZ) < 0) || 
		(AEE_IFile_Read(file, buffer, ENDHDRSIZ) != ENDHDRSIZ)) {
        goto loadFailed;
    }
    /* Set currentOffset to be the offset of buffer[0] */
    currentOffset = length - ENDHDRSIZ; 
    /* Set bp to be the location at which to start looking */
    bp = buffer;

    for (;;) {
        switch(bp[0]) {
            case '\006':   /* The header must start at least 3 bytes back */
                bp -= 3; break;
            case '\005':   /* The header must start at least 2 bytes back  */
                bp -= 2; break;
            case 'K':      /* The header must start at least 1 byte back  */
                bp -= 1; break;
            case 'P':
                if (bp[1] == 'K' && bp[2] == 5 && bp[3] == 6) {

                    int endpos = currentOffset + (bp - buffer);
                    if (endpos + ENDHDRSIZ + ENDCOM(bp) == length) {
						cenOffset = endpos - ENDSIZ(bp);
						locOffset = cenOffset - ENDOFF(bp);
						char flag[4];
                        if (AEE_IFile_Seek(file, ZM_SEEK_START, locOffset) >= 0 &&
							AEE_IFile_Read(file, flag, 4) == 4 &&
                            flag[0] == 'P' && flag[1] == 'K' && 
							flag[2] == 3   && flag[3] == 4) {
                     
						//	cenOffset = cenOffset;
						//	locOffset = locOffset;
							
                            goto next;
                        }
                        goto loadFailed;
                    }
                }
                /* FALL THROUGH */
            default:    
                bp -= 4;
        }
        if (bp < buffer) {
            int count = currentOffset - minOffset; /* Bytes left in file */
            if (count <= 0) {
                /* Nothing left to read.  Time to give up */
                goto loadFailed;
            } else {
                int available = (bufferSize - ENDHDRSIZ) + (buffer - bp);
                if (count > available) {
                    count = available;
                }
            }
            /* Back up, while keeping our virtual currentOffset the same */
            currentOffset -= count;
            bp += count;
            zmaee_memmove(buffer + count, buffer, bufferSize - count);
            if ((AEE_IFile_Seek(file, ZM_SEEK_START, currentOffset) < 0)
                   || (AEE_IFile_Read(file, buffer, count) != count)) {
               goto loadFailed;
            }

        }
    } /* end of for loop */

next:  
	mItemCount = 0;
	offset = cenOffset;
    for (;;) { 
		int nameLength;
        if ((AEE_IFile_Seek(file, ZM_SEEK_START, offset) < 0) || 
			(AEE_IFile_Read(file, buffer, CENHDRSIZ)!= CENHDRSIZ)) { 
            break;
        }
		
        nameLength = CENNAM(buffer);

		/*
        if (nameLength == filenameLength) {
            if (nameLength != (unsigned int)
				AEE_IFile_Read(file, buffer + CENHDRSIZ, nameLength)) {
                return 0;
            }
            if (0 == zmaee_memcmp(
				(void*)(buffer + CENHDRSIZ), 
				(void*)filename, nameLength)) {
                break;
            }
        }*/

        offset += CENHDRSIZ + nameLength + CENEXT(buffer) + CENCOM(buffer);
		++mItemCount;
    }
	if (mItemCount <= 0)
		goto loadFailed;


	mItemName = (char**)AEEAlloc::Alloc(sizeof(char*) * mItemCount);
	mItemOffset = (unsigned long*)AEEAlloc::Alloc(sizeof(unsigned long) * mItemCount);
	mItemSize = (unsigned long*)AEEAlloc::Alloc(sizeof(unsigned long) * mItemCount);
	zmaee_memset(mItemName, 0, sizeof(char*) * mItemCount);
	offset = cenOffset;
	for (i = 0; i < mItemCount; ++i) {
		int nameLength;
		char* name;
        if ((AEE_IFile_Seek(file, ZM_SEEK_START, offset) < 0) || 
			(AEE_IFile_Read(file, buffer, CENHDRSIZ)!= CENHDRSIZ)) { 
            goto loadFailed;
        }
		
        nameLength = CENNAM(buffer);
		mItemName[i] = name = (char*)AEEAlloc::Alloc(nameLength + 1);

        if (name == NULL || nameLength != (int)
			AEE_IFile_Read(file, name, nameLength)) {
            goto loadFailed;
        }
		name[nameLength] = 0;


		unsigned long decompLen = CENLEN(buffer); /* the decompressed length */
		unsigned long compLen   = CENSIZ(buffer); /* the compressed length */
		unsigned long method    = CENHOW(buffer); /* how it is stored */
		unsigned long expectedCRC = CENCRC(buffer); /* expected CRC */
	//    unsigned long actualCRC;
	//    unsigned char *result = NULL;

		unsigned char bufferItem[256];

		/* Make sure file is not encrypted */
		if ((CENFLG(buffer) & 1) == 1) {
			goto loadFailed;
		}

		/* This may cause a GC, so we have to extract out of "entry" all the
		 * info we need, before calling this.
		 */
	//    result = (unsigned char *)new jbyte[extraBytes + decompLen];
	//    if (result == NULL) {
	//        goto errorReturn;
	//    }

		if (/* Go to the beginning of the LOC header */
			(AEE_IFile_Seek(file, ZM_SEEK_START, locOffset + CENOFF(buffer)) < 0) 
			/* Read it */
			|| (AEE_IFile_Read(file, bufferItem, LOCHDRSIZ) != LOCHDRSIZ) 
			/* Skip over name and extension, if any */
			|| (AEE_IFile_Seek(file, ZM_SEEK_CURRENT, LOCNAM(bufferItem) + LOCEXT(bufferItem)) < 0)) {
			goto loadFailed;
		}


		switch (method) { 
			case STORED:
				/* The actual bits are right there in the file */
				if (compLen != decompLen) {
					goto loadFailed;
				}
				//AEE_IFile_Read(file, result + extraBytes, decompLen);
				break;

			case DEFLATED: {
				goto loadFailed;
				break;
			}
                
			default:
				/* Unknown method */
				goto loadFailed;
				break;
		}

		mItemSize[i] = decompLen;
		mItemOffset[i] = AEE_IFile_Tell(file);


        offset += CENHDRSIZ + nameLength + CENEXT(buffer) + CENCOM(buffer);

    }
	return;

loadFailed:
	Release();

}


AEEZipPackage::AEEZipPackage(const char* pathname,int startPos)
{

	AEE_IShell* shell = ZMAEE_GetShell();
	AEE_IFileMgr* filemgr;
	AEE_IFile* file;
    int offset; /* offset of first header */
	int i;
	long currentOffset, minOffset = 0;
    long length;
	
	unsigned const char *bp;
	
	unsigned char buffer[256];
	
	int bufferSize = sizeof(buffer);
	unsigned long locOffset;
	unsigned long cenOffset;
	
	mFile = NULL;
	mItemName = NULL;
	mItemOffset = NULL;
	mItemSize = NULL;
	
	AEE_IShell_CreateInstance(shell, ZM_AEE_CLSID_FILEMGR, (void**)&filemgr);
	mFile = file = AEE_IFileMgr_OpenFile(filemgr, pathname, ZM_OFM_READONLY);
	AEE_IFileMgr_Release(filemgr);
	if (file == NULL)
		goto loadFailed;

	mLoadSucc = true ;
	
	
	AEE_IFile_Seek(file, ZM_SEEK_END, 0);
    length = AEE_IFile_Tell(file) - startPos;
    AEE_IFile_Seek(file, ZM_SEEK_START, 0);
    if ((AEE_IFile_Seek(file, ZM_SEEK_END, -ENDHDRSIZ) < 0) || 
		(AEE_IFile_Read(file, buffer, ENDHDRSIZ) != ENDHDRSIZ)) {
        goto loadFailed;
    }
    /* Set currentOffset to be the offset of buffer[0] */
    currentOffset = length - ENDHDRSIZ; 
    /* Set bp to be the location at which to start looking */
    bp = buffer;

    for (;;) {
        switch(bp[0]) {
            case '\006':   /* The header must start at least 3 bytes back */
                bp -= 3; break;
            case '\005':   /* The header must start at least 2 bytes back  */
                bp -= 2; break;
            case 'K':      /* The header must start at least 1 byte back  */
                bp -= 1; break;
            case 'P':
                if (bp[1] == 'K' && bp[2] == 5 && bp[3] == 6) {

                    int endpos = currentOffset + (bp - buffer);
                    if (endpos + ENDHDRSIZ + ENDCOM(bp) == length) {
						cenOffset = endpos - ENDSIZ(bp);
						locOffset = cenOffset - ENDOFF(bp);
						char flag[4];
                        if (AEE_IFile_Seek(file, ZM_SEEK_START, locOffset+startPos) >= 0 &&
							AEE_IFile_Read(file, flag, 4) == 4 &&
                            flag[0] == 'P' && flag[1] == 'K' && 
							flag[2] == 3   && flag[3] == 4) {
                     
						//	cenOffset = cenOffset;
						//	locOffset = locOffset;
							
                            goto next;
                        }
                        goto loadFailed;
                    }
                }
                /* FALL THROUGH */
            default:    
                bp -= 4;
        }
        if (bp < buffer) {
            int count = currentOffset - minOffset; /* Bytes left in file */
            if (count <= 0) {
                /* Nothing left to read.  Time to give up */
                goto loadFailed;
            } else {
                int available = (bufferSize - ENDHDRSIZ) + (buffer - bp);
                if (count > available) {
                    count = available;
                }
            }
            /* Back up, while keeping our virtual currentOffset the same */
            currentOffset -= count;
            bp += count;
            zmaee_memmove(buffer + count, buffer, bufferSize - count);
            if ((AEE_IFile_Seek(file, ZM_SEEK_START, currentOffset+startPos) < 0)
                   || (AEE_IFile_Read(file, buffer, count) != count)) {
               goto loadFailed;
            }

        }
    } /* end of for loop */

next:  
	mItemCount = 0;
	offset = cenOffset;
    for (;;) { 
		int nameLength;
        if ((AEE_IFile_Seek(file, ZM_SEEK_START, offset+startPos) < 0) || 
			(AEE_IFile_Read(file, buffer, CENHDRSIZ)!= CENHDRSIZ)) { 
            break;
        }
		
        nameLength = CENNAM(buffer);

		/*
        if (nameLength == filenameLength) {
            if (nameLength != (unsigned int)
				AEE_IFile_Read(file, buffer + CENHDRSIZ, nameLength)) {
                return 0;
            }
            if (0 == zmaee_memcmp(
				(void*)(buffer + CENHDRSIZ), 
				(void*)filename, nameLength)) {
                break;
            }
        }*/

        offset += CENHDRSIZ + nameLength + CENEXT(buffer) + CENCOM(buffer);
		++mItemCount;
    }
	if (mItemCount <= 0)
		goto loadFailed;

	mItemName = (char**)AEEAlloc::Alloc(sizeof(char*) * mItemCount);
	mItemOffset = (unsigned long*)AEEAlloc::Alloc(sizeof(unsigned long) * mItemCount);
	mItemSize = (unsigned long*)AEEAlloc::Alloc(sizeof(unsigned long) * mItemCount);

	zmaee_memset(mItemName, 0, sizeof(char*) * mItemCount);
	offset = cenOffset;
	for (i = 0; i < mItemCount; ++i) {
		int nameLength;
		char* name;
        if ((AEE_IFile_Seek(file, ZM_SEEK_START, offset+startPos) < 0) || 
			(AEE_IFile_Read(file, buffer, CENHDRSIZ)!= CENHDRSIZ)) { 
            goto loadFailed;
        }
		
        nameLength = CENNAM(buffer);
		mItemName[i] = name = (char*)AEEAlloc::Alloc(nameLength + 1);

        if (name == NULL || nameLength != (int)
			AEE_IFile_Read(file, name, nameLength)) {
            goto loadFailed;
        }
		name[nameLength] = 0;


		unsigned long decompLen = CENLEN(buffer); /* the decompressed length */
		unsigned long compLen   = CENSIZ(buffer); /* the compressed length */
		unsigned long method    = CENHOW(buffer); /* how it is stored */
		unsigned long expectedCRC = CENCRC(buffer); /* expected CRC */
	//    unsigned long actualCRC;
	//    unsigned char *result = NULL;

		unsigned char bufferItem[256];

		/* Make sure file is not encrypted */
		if ((CENFLG(buffer) & 1) == 1) {
			goto loadFailed;
		}

		/* This may cause a GC, so we have to extract out of "entry" all the
		 * info we need, before calling this.
		 */
	//    result = (unsigned char *)new jbyte[extraBytes + decompLen];
	//    if (result == NULL) {
	//        goto errorReturn;
	//    }

		if (/* Go to the beginning of the LOC header */
			(AEE_IFile_Seek(file, ZM_SEEK_START, locOffset + CENOFF(buffer)+startPos) < 0) 
			/* Read it */
			|| (AEE_IFile_Read(file, bufferItem, LOCHDRSIZ) != LOCHDRSIZ) 
			/* Skip over name and extension, if any */
			|| (AEE_IFile_Seek(file, ZM_SEEK_CURRENT, LOCNAM(bufferItem) + LOCEXT(bufferItem)) < 0)) {
			goto loadFailed;
		}


		switch (method) { 
			case STORED:
				/* The actual bits are right there in the file */
				if (compLen != decompLen) {
					goto loadFailed;
				}
				//AEE_IFile_Read(file, result + extraBytes, decompLen);
				break;

			case DEFLATED: {
				goto loadFailed;
				break;
			}
                
			default:
				/* Unknown method */
				goto loadFailed;
				break;
		}

		mItemSize[i] = decompLen;
		mItemOffset[i] = AEE_IFile_Tell(file);


        offset += CENHDRSIZ + nameLength + CENEXT(buffer) + CENCOM(buffer);

    }
	return;

loadFailed:
	Release();

}


int AEEZipPackage::UnZipItem(const char* name,const char* dir,char* tmpbuf,int bufSize)
{
	AEE_IFile* pDesFile = NULL ;
	char desPath[128] = {0};
	int index = GetIndex(name);
	int size;
	int ret = 0;
	if (index < 0 || index >= mItemCount ||
		zmaee_strlen(dir) + zmaee_strlen(name) > sizeof(desPath))
		return -1;
	
	size = GetItemSize(index);
	if (size < 0 || AEE_IFile_Seek(mFile, ZM_SEEK_START, mItemOffset[index]) < 0)
		return -1;
	zmaee_sprintf(desPath,"%s%s",dir,name);
	AEE_IFileMgr* filemgr = NULL;

	if(AEE_IShell_CreateInstance(ZMAEE_GetShell(),ZM_AEE_CLSID_FILEMGR,(void**)&filemgr)
		== E_ZM_AEE_SUCCESS)
	{
		
		int leftSize = size;
		int nRead = 0;
		pDesFile = AEE_IFileMgr_OpenFile(filemgr,desPath,ZM_OFM_TRUNCATE|ZM_OFM_READWRITE);
		if (pDesFile)
		{
			int nTry = 0;
			while (leftSize > 0)
			{
				nRead = leftSize > bufSize ? bufSize : leftSize;
				nRead = AEE_IFile_Read(mFile, tmpbuf, nRead);
				if(nRead > 0)
				{
					if(AEE_IFile_Write(pDesFile,tmpbuf,nRead) <=0)
					{
						nTry++;
						if(nTry > 5)
						{
							break ;
							ret = -1 ;
						}
						continue;
					}
					else
					{
						nTry = 0;
					}
				}
				leftSize -= nRead;
			}
			AEE_IFile_Release(pDesFile);
		}
		AEE_IFileMgr_Release(filemgr);
	}

	return ret;
}


