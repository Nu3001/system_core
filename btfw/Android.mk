ifneq ($(BUILD_TINY_ANDROID),true)
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := \
    bionic \
    external/stlport/stlport
    
LOCAL_SRC_FILES:= \
     	$(call all-proto-files-under, .) \
    	BTUpdateClientSocket.cpp \
    	ClientSocket.cpp \
    	Socket.cpp \
	main.cpp \
	flc_dfu.cpp \
	platformSpecific.cpp \
	SerialPort.cpp	\
	ubcsp.cpp

LOCAL_MODULE:= btfw

LOCAL_SHARED_LIBRARIES := \
    libstlport \

include $(BUILD_EXECUTABLE)
endif
