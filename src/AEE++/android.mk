
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libaeeplus
LOCAL_SRC_FILES :=\
./AEEBase.cpp\
./AEEBaseApp.cpp\
./AEEDelegate.cpp\
./AEEFile.cpp\
./AEEFileFind.cpp\
./AEEFileMgr.cpp\
./AEEHttp.cpp\
./AEEMain.cpp\
./AEESocket.cpp\
./AEEStackAlloc.cpp\
charge/AeeCharge.cpp\
charge/AeeChargePage.cpp\
charge/AeeTextbox.cpp\
dui/dui_animation.cpp\
dui/dui_app.cpp\
dui/dui_basewnd.cpp\
dui/dui_bdfiller.cpp\
dui/dui_bgfiller.cpp\
dui/dui_bitmaptable.cpp\
dui/dui_builder.cpp\
dui/dui_button.cpp\
dui/dui_checkbox.cpp\
dui/dui_container.cpp\
dui/dui_control.cpp\
dui/dui_dc.cpp\
dui/dui_edit.cpp\
dui/dui_fillertable.cpp\
dui/dui_image.cpp\
dui/dui_indicator.cpp\
dui/dui_label.cpp\
dui/dui_manager.cpp\
dui/dui_pages.cpp\
dui/dui_progress.cpp\
dui/dui_radio.cpp\
dui/dui_richtext.cpp\
dui/dui_spacer.cpp\
dui/dui_stringtable.cpp\
dui/dui_stringutil.cpp\
dui/dui_waitcursor.cpp\
dui/dui_window.cpp\
dui/layout/dui_layout_border.cpp\
dui/layout/dui_layout_grid.cpp\
dui/layout/dui_layout_hbox.cpp\
dui/layout/dui_layout_relative.cpp\
dui/layout/dui_layout_tab.cpp\
dui/layout/dui_layout_vbox.cpp\
dui/layout/dui_layout_vflow.cpp\
dui/scroll/dui_scroll.cpp\
dui/scroll/lau_scroller.c\
dui/scroll/lau_velocity_tracker.c\
game/AEEGameBody.cpp\
game/AEEGameCanvas.cpp\
game/AEEGameMap.cpp\
game/AEEGameObject.cpp\
game/AEEGameObjectManage.cpp\
game/AEEGameScene.cpp\
game/AEEGameSound.cpp\
game/AEEGameWorld.cpp\
game/AEESprite.cpp\
gdi/AEERotateBitmap.cpp\
json/AEEJsonArray.cpp\
json/AEEJsonBuilder.cpp\
json/AEEJsonObject.cpp\
json/AEEJsonParser.cpp\
json/AEEJsonRpc.cpp\
json/AEEJsonStream.cpp\
util/AEEBase64.cpp\
util/AEEDes.cpp\
util/AEEMd5.cpp\
xml/AEEXmlParser.cpp


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../zmaee/include         \
                    $(LOCAL_PATH)/charge                   \
                    $(LOCAL_PATH)/dui                      \
                    $(LOCAL_PATH)/game                     \
                    $(LOCAL_PATH)/gdi                      \
                    $(LOCAL_PATH)/json                     \
                    $(LOCAL_PATH)/util                     \
                    $(LOCAL_PATH)/xml                      \
                    $(LOCAL_PATH)
                    

include $(BUILD_STATIC_LIBRARY)
