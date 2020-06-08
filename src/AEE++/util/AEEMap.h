#ifndef _AEE_MAP_H_
#define _AEE_MAP_H_
#include "AEEBase.h"
template<typename KEY, typename DATA>
class AEEMap
{  
	public:
		struct Pair
		{
			Pair(const Pair& src):first(src.first), second(src.second){
			}
			
			Pair& operator=(const Pair& src){
				first = src.first;
				second = src.second;
				return *this;
			}
			KEY		first;
			DATA	second;
		};
 
        enum COLOR{RED,BLACK};  
        struct RB_Node  
        {  
        public:  
            RB_Node()  
            {  
                //RB_COLOR = BLACK;  
                right = NULL;  
                left = NULL;  
                parent = NULL;  
            } 

			void Init()
			{ 
                right = NULL;  
                left = NULL;  
                parent = NULL;  
			}

            COLOR RB_COLOR;  
            RB_Node* right;  
            RB_Node* left;  
            RB_Node* parent;  
			Pair	 pair;
        };  

	public:		
		class iterator{
		public:
			iterator():m_ptr(0),m_nullNode(0){
			}
			iterator(RB_Node* p, RB_Node* nullNode):m_ptr(p),m_nullNode(nullNode){
			}
			iterator(const iterator& src) :m_ptr(src.m_ptr),m_nullNode(src.m_nullNode){
			}
			iterator& operator=(const iterator& src){
				m_ptr = src.m_ptr;
				m_nullNode = src.m_nullNode;
				return *this;
			}
			inline Pair& operator*(){
				return m_ptr->pair;
			}
			inline Pair* operator->(){
				return   &(operator*());
			}

			inline iterator& operator++(){
				m_ptr = next();
				return (*this);
			}
			inline iterator operator++(int){
				iterator _Tmp = *this;
				++*this;
				return (_Tmp);
			}
			inline iterator& operator--(){
				m_ptr = prev();
				return (*this);
			}
			inline iterator operator--(int){
				iterator _Tmp = *this;
				--*this;
				return (_Tmp);
			}
			inline bool operator==(const iterator& x) const{
				return (x.m_ptr == m_ptr);
			}
			inline bool operator!=(const iterator& x) const{
				return (x.m_ptr != m_ptr);
			}

			RB_Node* next()
			{
				RB_Node *parent;

				if (m_ptr->parent == m_ptr)
						return (RB_Node*)m_nullNode;

				/* If we have a right-hand child, go down and then left as far
				   as we can. */
				if (m_ptr->right != m_nullNode) 
				{
					m_ptr = m_ptr->right; 
					while (m_ptr->left != m_nullNode)
							m_ptr = m_ptr->left;
					return m_ptr;
				}

				/* No right-hand children.  Everything down and left is
				   smaller than us, so any 'next' node must be in the general
				   direction of our parent. Go up the tree; any time the
				   ancestor is a right-hand child of its parent, keep going
				   up. First time it's a left-hand child of its parent, said
				   parent is our 'next' node. */
				while ((parent = m_ptr->parent) != m_nullNode && m_ptr == parent->right)
						m_ptr = parent;
				return parent;
			}
			RB_Node* prev()
			{
				RB_Node* parent;
				if (m_ptr->parent == m_ptr)
					return (RB_Node*)m_nullNode;
				if (m_ptr->left != m_nullNode)
				{
					m_ptr = m_ptr->left;
					while (m_ptr->right)
						m_ptr = m_ptr->right;
					return m_ptr;

				}

				while ((parent = m_ptr->parent) != m_nullNode && m_ptr == parent->left) 
					m_ptr = parent;
				return parent;
			}

			RB_Node* m_ptr;
			RB_Node* m_nullNode;
		};

	public:  
		AEEMap()  
		{  
			this->m_nullNode = (RB_Node*)AEEAlloc::Alloc(sizeof(RB_Node));  
			this->m_nullNode->Init();
			this->m_root = m_nullNode;  
			this->m_nullNode->right = this->m_root;  
			this->m_nullNode->left = this->m_root;  
			this->m_nullNode->parent = this->m_root;  
			this->m_nullNode->RB_COLOR = BLACK;  
			this->m_count = 0;
		}  
          
		bool empty()  
		{  
			if(this->m_root == this->m_nullNode)  
			{  
				return true;  
			}  
			else  
			{  
				return false;  
			}  
		} 

		int size() const {
			return this->m_count;
		}

		void clear()
		{
			clear(m_root);
			m_root = m_nullNode;
			m_count = 0;
		}
			
		iterator begin(){
			return iterator(first(), m_nullNode);
		}
		
		iterator rbegin(){
			return iterator(last(), m_nullNode);
		}
		
		iterator end(){
			return iterator(m_nullNode, m_nullNode);
		} 
          
		//����key  
		iterator find(KEY& key)  
		{  
			RB_Node* index = m_root;  
			while(index != m_nullNode)  
			{  
				if(key < index->pair.first)     
				{  
					index  = index->left;  //�ȵ�ǰ��С������  
				}  
				else if(key > index->pair.first)  
				{  
					index = index->right;  //�ȵ�ǰ�Ĵ�����  
				}  
				else  
				{  
					return iterator(index, m_nullNode);
				}  
			}  
			return iterator(m_nullNode, m_nullNode);  
		}

		bool erase(iterator it)
		{
			RB_Node* delete_point = it.m_ptr;  
			if(delete_point == m_nullNode)  
			{  
				return false;  
			}  
			if(delete_point->left!=m_nullNode && delete_point->right!=m_nullNode)  
			{  
				RB_Node* successor = InOrderSuccessor(delete_point);  
				delete_point->pair.second = successor->pair.second;  
				delete_point->pair.first = successor->pair.first;  
				delete_point = successor;  
			}  
			RB_Node* delete_point_child;  
			if(delete_point->right!=m_nullNode)  
			{  
				delete_point_child = delete_point->right;  
			}  
			else if(delete_point->left!=m_nullNode)  
			{  
				delete_point_child = delete_point->left;  
			}  
			else  
			{  
				delete_point_child = m_nullNode;  
			}  
			delete_point_child->parent = delete_point->parent;  
			if(delete_point->parent==m_nullNode)//delete root node  
			{  
				m_root = delete_point_child;  
				m_nullNode->parent = m_root;  
				m_nullNode->left = m_root;  
				m_nullNode->right = m_root;  
			}  
			else if(delete_point == delete_point->parent->right)  
			{  
				delete_point->parent->right = delete_point_child;  
			}  
			else  
			{  
				delete_point->parent->left = delete_point_child;  
			}  
			if(delete_point->RB_COLOR==BLACK && !(delete_point_child==m_nullNode && delete_point_child->parent==m_nullNode))  
			{  
				DeleteFixUp(delete_point_child);  
			}  
			AEEAlloc::Free(delete_point);  
			m_count--;
			return true;  
		}
		
		bool erase(KEY& key)  
		{  
			iterator it = find(key);
			return erase(it);
		}    
		
		bool insert(KEY& key,DATA& data)  
		{  
			RB_Node* insert_point = m_nullNode;  
			RB_Node* index = m_root;  
			while(index!=m_nullNode)  
			{  
				insert_point = index;  
				if(key<index->pair.first)  
				{  
					index = index->left;  
				}  
				else if(key > index->pair.first)  
				{  
					index = index->right;  
				}  
				else  
				{  
					return false;  
				}  
			}  
			RB_Node* insert_node = (RB_Node*)AEEAlloc::Alloc(sizeof(RB_Node));
			insert_node->Init();
			m_count++;
			insert_node->pair.first = key;  
			insert_node->pair.second = data;  
			insert_node->RB_COLOR = RED;  
			insert_node->right = m_nullNode;  
			insert_node->left = m_nullNode;  
			if(insert_point==m_nullNode) //����������һ�ſ���  
			{  
				m_root = insert_node;  
				m_root->parent = m_nullNode;  
				m_nullNode->left = m_root;  
				m_nullNode->right = m_root;  
				m_nullNode->parent = m_root;  
			}  
			else  
			{  
				if(key<insert_point->pair.first)  
				{  
					insert_point->left = insert_node;  
				}  
				else  
				{  
					insert_point->right = insert_node;  
				}  
				insert_node->parent = insert_point;  
			}  
			InsertFixUp(insert_node);    //����InsertFixUp�޸���������ʡ�  
			return true;
		} 
		
		virtual ~AEEMap()  
		{  
			clear(m_root);  
			AEEAlloc::Free(m_nullNode);  
		}  
		
	private:    
      
		//---------------------�����������޸�--------------------------------  
		//3�ֲ�����������������α������(δ�漰������ȫ���Ĳ������)��  
		/* 
		RB-INSERT-FIXUP(T, z) 
		1 while color[p[z]] = RED 
		2     do if p[z] = left[p[p[z]]] 
		3           then y �� right[p[p[z]]] 
		4                if color[y] = RED 
		5                   then color[p[z]] �� BLACK                    ? Case 1 
		6                        color[y] �� BLACK                       ? Case 1 
		7                        color[p[p[z]]] �� RED                   ? Case 1 
		8                        z �� p[p[z]]                            ? Case 1 
		9                   else if z = right[p[z]] 
		10                           then z �� p[z]                       ? Case 2 
		11                                LEFT-ROTATE(T, z)              ? Case 2 
		12                           color[p[z]] �� BLACK                 ? Case 3 
		13                           color[p[p[z]]] �� RED                ? Case 3 
		14                           RIGHT-ROTATE(T, p[p[z]])            ? Case 3 
		15           else (same as then clause with "right" and "left" exchanged) 
		16 color[root[T]] �� BLACK 
		*/  
		//Ȼ��Ĺ������ͷǳ����ˣ���������α�����дΪ������c++���룺  
		void InsertFixUp(RB_Node* node)  
		{  
			while(node->parent->RB_COLOR==RED)  
			{  
				if(node->parent==node->parent->parent->left)   //  
				{  
					RB_Node* uncle = node->parent->parent->right;  
					if(uncle->RB_COLOR == RED)   //�������1��z������y�Ǻ�ɫ�ġ�  
					{  
						node->parent->RB_COLOR = BLACK;  
						uncle->RB_COLOR = BLACK;  
						node->parent->parent->RB_COLOR = RED;  
						node = node->parent->parent;  
					}  
					else if(uncle->RB_COLOR == BLACK )  //�������2��z������y�Ǻ�ɫ�ģ���  
					{  
						if(node == node->parent->right) //��z���Һ���  
						{  
							node = node->parent;  
							RotateLeft(node);  
						}  
						else                 //�������3��z������y�Ǻ�ɫ�ģ���z�����ӡ�  
						{  
							node->parent->RB_COLOR = BLACK;  
							node->parent->parent->RB_COLOR = RED;  
							RotateRight(node->parent->parent);  
						}  
					}  
				}  
				else //�ⲿ�������Ϊ�������1�У�z�ĸ���������Ϊ�游���Һ����˵��������д�ġ�  
					//15 else (same as then clause with "right" and "left" exchanged)  
				{  
					RB_Node* uncle = node->parent->parent->left;  
					if(uncle->RB_COLOR == RED)  
					{  
						node->parent->RB_COLOR = BLACK;  
						uncle->RB_COLOR = BLACK;  
						uncle->parent->RB_COLOR = RED;  
						node = node->parent->parent;  
					}  
					else if(uncle->RB_COLOR == BLACK)  
					{  
						if(node == node->parent->left)  
						{  
							node = node->parent;  
							RotateRight(node);     //������������ȣ�������Ϊ����  
						}  
						else  
						{  
							node->parent->RB_COLOR = BLACK;  
							node->parent->parent->RB_COLOR = RED;  
							RotateLeft(node->parent->parent);   //������Ϊ���������ɡ�  
						}  
					}  
				}  
			}  
			m_root->RB_COLOR = BLACK;  
		}  
      
		//��������ʵ��  
		bool RotateLeft(RB_Node* node)  
		{  
			if(node==m_nullNode || node->right==m_nullNode)  
			{  
				return false;//can't rotate  
			}  
			RB_Node* lower_right = node->right;  
			lower_right->parent =  node->parent;  
			node->right=lower_right->left;  
			if(lower_right->left!=m_nullNode)  
			{  
				lower_right->left->parent = node;  
			}  
			if(node->parent==m_nullNode) //rotate node is root  
			{  
				m_root = lower_right;  
				m_nullNode->left = m_root;  
				m_nullNode->right= m_root;  
				//m_nullNode->parent = m_root;  
			}  
			else  
			{  
				if(node == node->parent->left)  
				{  
					node->parent->left = lower_right;  
				}  
				else  
				{  
					node->parent->right = lower_right;  
				}  
			}  
			node->parent = lower_right;  
			lower_right->left = node;  
			return true;
		}  
      
		//��������ʵ��  
		bool RotateRight(RB_Node* node)  
		{  
			if(node==m_nullNode || node->left==m_nullNode)  
			{  
				return false;//can't rotate  
			}  
			RB_Node* lower_left = node->left;  
			node->left = lower_left->right;  
			lower_left->parent = node->parent;  
			if(lower_left->right!=m_nullNode)  
			{  
				lower_left->right->parent = node;  
			}  
			if(node->parent == m_nullNode) //node is root  
			{  
				m_root = lower_left;  
				m_nullNode->left = m_root;  
				m_nullNode->right = m_root;  
				//m_nullNode->parent = m_root;  
			}  
			else  
			{  
				if(node==node->parent->right)  
				{  
					node->parent->right = lower_left;  
				}  
				else  
				{  
					node->parent->left = lower_left;  
				}  
			}  
			node->parent = lower_left;  
			lower_left->right = node;  
			return true;
		}  
      
		//---------------------ɾ����������޸�-----------------------------------  
		//���еĹ�������������23��α�����У�  
		/* 
		RB-DELETE-FIXUP(T, x) 
		1 while x �� root[T] and color[x] = BLACK 
		2     do if x = left[p[x]] 
		3           then w �� right[p[x]] 
		4                if color[w] = RED 
		5                   then color[w] �� BLACK                        ?  Case 1 
		6                        color[p[x]] �� RED                       ?  Case 1 
		7                        LEFT-ROTATE(T, p[x])                    ?  Case 1 
		8                        w �� right[p[x]]                         ?  Case 1 
		9                if color[left[w]] = BLACK and color[right[w]] = BLACK 
		10                   then color[w] �� RED                          ?  Case 2 
		11                        x p[x]                                  ?  Case 2 
		12                   else if color[right[w]] = BLACK 
		13                           then color[left[w]] �� BLACK          ?  Case 3 
		14                                color[w] �� RED                  ?  Case 3 
		15                                RIGHT-ROTATE(T, w)              ?  Case 3 
		16                                w �� right[p[x]]                 ?  Case 3 
		17                         color[w] �� color[p[x]]                 ?  Case 4 
		18                         color[p[x]] �� BLACK                    ?  Case 4 
		19                         color[right[w]] �� BLACK                ?  Case 4 
		20                         LEFT-ROTATE(T, p[x])                   ?  Case 4 
		21                         x �� root[T]                            ?  Case 4 
		22        else (same as then clause with "right" and "left" exchanged) 
		23 color[x] �� BLACK  
		*/  
		//�������Ĺ������ܼ򵥣���������α�����д��c++���뼴�ɡ�  
		void DeleteFixUp(RB_Node* node)  
		{  
			while(node!=m_root && node->RB_COLOR==BLACK)  
			{  
				if(node == node->parent->left)  
				{  
					RB_Node* brother = node->parent->right;  
					if(brother->RB_COLOR==RED)   //���1��x���ֵ�w�Ǻ�ɫ�ġ�  
					{  
						brother->RB_COLOR = BLACK;  
						node->parent->RB_COLOR = RED;  
						RotateLeft(node->parent);  
					}  
					else     //���2��x���ֵ�w�Ǻ�ɫ�ģ�  
					{  
						if(brother->left->RB_COLOR == BLACK && brother->right->RB_COLOR == BLACK)  
							//��w���������Ӷ��Ǻ�ɫ�ġ�  
						{  
							brother->RB_COLOR = RED;  
							node = node->parent;  
						}  
						else if(brother->right->RB_COLOR == BLACK)  
							//���3��x���ֵ�w�Ǻ�ɫ�ģ�w���Һ����Ǻ�ɫ��w�������Ǻ�ɫ����  
						{  
							brother->RB_COLOR = RED;  
							brother->left->RB_COLOR = BLACK;  
							RotateRight(brother);  
						}  
						else if(brother->right->RB_COLOR == RED)  
							//���4��x���ֵ�w�Ǻ�ɫ�ģ���w���Һ���ʱ��ɫ�ġ�  
						{  
							brother->RB_COLOR = node->parent->RB_COLOR;  
							node->parent->RB_COLOR = BLACK;  
							brother->right->RB_COLOR = BLACK;  
							RotateLeft(node->parent);  
							node = m_root;  
						}  
					}  
				}  
				else  //������������������1�У�node��Ϊ�Һ��Ӷ������ġ�  
					//22        else (same as then clause with "right" and "left" exchanged)  
					//ͬ����ԭ��һ�£�ֻ������������Ϊ����������������Ϊ���������ɡ��������벻�䡣  
				{  
					RB_Node* brother = node->parent->left;  
					if(brother->RB_COLOR == RED)  
					{  
						brother->RB_COLOR = BLACK;  
						node->parent->RB_COLOR = RED;  
						RotateRight(node->parent);  
					}  
					else  
					{  
						if(brother->left->RB_COLOR==BLACK && brother->right->RB_COLOR == BLACK)  
						{  
							brother->RB_COLOR = RED;  
							node = node->parent;  
						}  
						else if(brother->left->RB_COLOR==BLACK)  
						{  
							brother->RB_COLOR = RED;  
							brother->right->RB_COLOR = BLACK;  
							RotateLeft(brother);  
						}  
						else if(brother->left->RB_COLOR==RED)  
						{  
							brother->RB_COLOR = node->parent->RB_COLOR;  
							node->parent->RB_COLOR = BLACK;  
							brother->left->RB_COLOR = BLACK;  
							RotateRight(node->parent);  
							node = m_root;  
						}  
					}  
				}  
			}  
			m_nullNode->parent = m_root;   //���node��Ϊ����㣬  
			node->RB_COLOR = BLACK;    //����Ϊ��ɫ��  
		}  
      
		//  
		inline RB_Node* InOrderPredecessor(RB_Node* node)  
		{  
			if(node==m_nullNode)       //null node has no predecessor  
			{  
				return m_nullNode;  
			}  
			RB_Node* result = node->left;     //get node's left child  
			while(result!=m_nullNode)         //try to find node's left subtree's right most node  
			{  
				if(result->right!=m_nullNode)       
				{  
					result = result->right;  
				}  
				else  
				{  
					break;  
				}  
			}            //after while loop result==null or result's right child is null  
			if(result==m_nullNode)  
			{  
				RB_Node* index = node->parent;  
				result = node;  
				while(index!=m_nullNode && result == index->left)  
				{  
					result = index;  
					index = index->parent;  
				}  
				result = index;         // first right parent or null  
			}  
			return result;  
		}  
      
		//  
		inline RB_Node* InOrderSuccessor(RB_Node* node)  
		{  
			if(node==m_nullNode)       //null node has no successor  
			{  
				return m_nullNode;  
			}  
			RB_Node* result = node->right;   //get node's right node  
			while(result!=m_nullNode)        //try to find node's right subtree's left most node  
			{  
				if(result->left!=m_nullNode)       
				{  
					result = result->left;  
				}  
				else  
				{  
					break;  
				}  
			}                              //after while loop result==null or result's left child is null  
			if(result == m_nullNode)  
			{  
				RB_Node* index = node->parent;  
				result = node;  
				while(index!=m_nullNode && result == index->right)  
				{  
					result = index;  
					index = index->parent;  
				}  
				result = index;         //first parent's left or null  
			}  
			return result;  
		}  
		void InOrderTraverse(RB_Node* node)  
		{  
			if(node==m_nullNode)  
			{  
				return;  
			}  
			else  
			{  
				InOrderTraverse(node->left); 
				InOrderTraverse(node->right);  
			}  
		} 

		// utility function for destructor to destruct object;  
		void clear(RB_Node* node)  
		{  
			if(node==m_nullNode)  
			{  
				return ;  
			}  
			else  
			{  
				clear(node->left);  
				clear(node->right);  
				AEEAlloc::Free(node);  
				m_count--;
			}  
		}  
		RB_Node* first()
		{
			if (m_root == m_nullNode) 
				return m_nullNode;	
			RB_Node* n = m_root;
			while (n->left != m_nullNode)
				n = n->left;
			return n;
		}

		RB_Node* last()
		{
			if (m_root == m_nullNode)
				return m_nullNode;
			RB_Node* n = m_root;
			while (n->right != m_nullNode)
				n = n->right;
			return n;
		}
		
	private:  
		RB_Node *m_nullNode;  
		RB_Node *m_root;  
		int		 m_count;
};  

#endif	/*_AEE_MAP_H_*/