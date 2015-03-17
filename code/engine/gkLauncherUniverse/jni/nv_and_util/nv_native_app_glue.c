//----------------------------------------------------------------------------------
// File:        jni/nv_and_util/nv_native_app_glue.c
// SDK Version: v10.14 
// Email:       tegradev@nvidia.com
// Site:        http://developer.nvidia.com/
//
//
// Copyright (C) 2010 The Android Open Source Project
// Copyright (c) 2011, NVIDIA CORPORATION.  All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//
//----------------------------------------------------------------------------------
/* NVIDIA has modified this Android file to extend the behavior.  The original license
 * is provided above for reference and legal purposes.  Where possible, original
 * code is left as-is for easy updates.  NVIDIA extensions, where possible, use
 * nv_app_/NV_ prefixing instead of android_app_/APP_ prefixing
 */

#include <jni.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include "nv_native_app_glue.h"
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "nv_native_app_glue", __VA_ARGS__))
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, "nv_native_app_glue", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "nv_native_app_glue", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "nv_native_app_glue", __VA_ARGS__))

enum
{
    // Android lifecycle status flags.  Not app-specific
    // Set between onCreate and onDestroy
    NV_APP_STATUS_RUNNING          = 0x00000001,
    // Set between onResume and onPause
    NV_APP_STATUS_ACTIVE           = 0x00000002,
    // Set between onWindowFocusChanged(true) and (false)
    NV_APP_STATUS_FOCUSED          = 0x00000004,
    // Set when the app's SurfaceHolder points to a 
    // valid, nonzero-sized surface
    NV_APP_STATUS_HAS_REAL_SURFACE  = 0x00000008,

    // Mask of all app lifecycle status flags, useful for checking when is it
    // a reasonable time to be setting up EGL and rendering
    NV_APP_STATUS_INTERACTABLE     = 0x0000000f
};


static void free_saved_state(struct android_app* android_app) {
    pthread_mutex_lock(&android_app->mutex);
    if (android_app->savedState != NULL) {
        free(android_app->savedState);
        android_app->savedState = NULL;
        android_app->savedStateSize = 0;
    }
    pthread_mutex_unlock(&android_app->mutex);
}

int8_t android_app_read_cmd(struct android_app* android_app) {
    int8_t cmd;
    if (read(android_app->msgread, &cmd, sizeof(cmd)) == sizeof(cmd)) {
        switch (cmd) {
            case APP_CMD_SAVE_STATE:
                free_saved_state(android_app);
                break;
        }
        return cmd;
    } else {
        LOGI("No data on command pipe!");
    }
    return -1;
}

static void print_cur_config(struct android_app* android_app) {
    char lang[2], country[2];
    AConfiguration_getLanguage(android_app->config, lang);
    AConfiguration_getCountry(android_app->config, country);

    LOGI("Config: mcc=%d mnc=%d lang=%c%c cnt=%c%c orien=%d touch=%d dens=%d "
            "keys=%d nav=%d keysHid=%d navHid=%d sdk=%d size=%d long=%d "
            "modetype=%d modenight=%d",
            AConfiguration_getMcc(android_app->config),
            AConfiguration_getMnc(android_app->config),
            lang[0], lang[1], country[0], country[1],
            AConfiguration_getOrientation(android_app->config),
            AConfiguration_getTouchscreen(android_app->config),
            AConfiguration_getDensity(android_app->config),
            AConfiguration_getKeyboard(android_app->config),
            AConfiguration_getNavigation(android_app->config),
            AConfiguration_getKeysHidden(android_app->config),
            AConfiguration_getNavHidden(android_app->config),
            AConfiguration_getSdkVersion(android_app->config),
            AConfiguration_getScreenSize(android_app->config),
            AConfiguration_getScreenLong(android_app->config),
            AConfiguration_getUiModeType(android_app->config),
            AConfiguration_getUiModeNight(android_app->config));
}

void android_app_pre_exec_cmd(struct android_app* android_app, int8_t cmd) {
    switch (cmd) {
        case APP_CMD_INPUT_CHANGED:
            LOGI("APP_CMD_INPUT_CHANGED\n");
            pthread_mutex_lock(&android_app->mutex);
            if (android_app->inputQueue != NULL) {
                AInputQueue_detachLooper(android_app->inputQueue);
            }
            android_app->inputQueue = android_app->pendingInputQueue;
            if (android_app->inputQueue != NULL) {
                LOGI("Attaching input queue to looper");
                AInputQueue_attachLooper(android_app->inputQueue,
                        android_app->looper, LOOPER_ID_INPUT, NULL,
                        &android_app->inputPollSource);
            }
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW\n");
            pthread_mutex_lock(&android_app->mutex);
            android_app->window = android_app->pendingWindow;
			if (android_app->window && 
				ANativeWindow_getWidth(android_app->window) &&
				ANativeWindow_getHeight(android_app->window))
			{
				android_app->lifecycleFlags |= NV_APP_STATUS_HAS_REAL_SURFACE;
			}
			else
			{
				android_app->lifecycleFlags &= ~NV_APP_STATUS_HAS_REAL_SURFACE;
			}

            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW\n");
			android_app->lifecycleFlags &= ~NV_APP_STATUS_HAS_REAL_SURFACE;
            pthread_cond_broadcast(&android_app->cond);
            break;

		case APP_CMD_WINDOW_RESIZED:
            LOGI("APP_CMD_WINDOW_RESIZED\n");
            pthread_mutex_lock(&android_app->mutex);
			if (android_app->window && 
				ANativeWindow_getWidth(android_app->window) &&
				ANativeWindow_getHeight(android_app->window))
			{
				android_app->lifecycleFlags |= NV_APP_STATUS_HAS_REAL_SURFACE;
			}
			else
			{
				android_app->lifecycleFlags &= ~NV_APP_STATUS_HAS_REAL_SURFACE;
			}

            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS\n");
            pthread_mutex_lock(&android_app->mutex);
			android_app->lifecycleFlags |= NV_APP_STATUS_FOCUSED;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS\n");
            pthread_mutex_lock(&android_app->mutex);
			android_app->lifecycleFlags &= ~NV_APP_STATUS_FOCUSED;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_RESUME:
            LOGI("activityState=%d\n", cmd);
            pthread_mutex_lock(&android_app->mutex);
			android_app->lifecycleFlags |= NV_APP_STATUS_ACTIVE;
            android_app->activityState = cmd;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_START:
            LOGI("activityState=%d\n", cmd);
            pthread_mutex_lock(&android_app->mutex);
            android_app->activityState = cmd;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_PAUSE:
            LOGI("activityState=%d\n", cmd);
            pthread_mutex_lock(&android_app->mutex);
			android_app->lifecycleFlags &= ~NV_APP_STATUS_ACTIVE;
            android_app->activityState = cmd;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_STOP:
            LOGI("activityState=%d\n", cmd);
            pthread_mutex_lock(&android_app->mutex);
            android_app->activityState = cmd;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_CONFIG_CHANGED:
            LOGI("APP_CMD_CONFIG_CHANGED\n");
            AConfiguration_fromAssetManager(android_app->config,
                    android_app->activity->assetManager);
            print_cur_config(android_app);
            break;

        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY\n");
			android_app->lifecycleFlags &= ~NV_APP_STATUS_RUNNING;
            break;
    }
}

void android_app_post_exec_cmd(struct android_app* android_app, int8_t cmd) {
    switch (cmd) {
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW\n");
            pthread_mutex_lock(&android_app->mutex);
            android_app->window = NULL;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_SAVE_STATE:
            LOGI("APP_CMD_SAVE_STATE\n");
            pthread_mutex_lock(&android_app->mutex);
            android_app->stateSaved = 1;
            pthread_cond_broadcast(&android_app->cond);
            pthread_mutex_unlock(&android_app->mutex);
            break;

        case APP_CMD_RESUME:
            free_saved_state(android_app);
            break;
    }
}

void app_dummy() {

}

static void android_app_destroy(struct android_app* android_app) {
    LOGI("android_app_destroy!");
    free_saved_state(android_app);
    pthread_mutex_lock(&android_app->mutex);
    if (android_app->inputQueue != NULL) {
        AInputQueue_detachLooper(android_app->inputQueue);
    }
    AConfiguration_delete(android_app->config);
    android_app->destroyed = 1;
    pthread_cond_broadcast(&android_app->cond);
    pthread_mutex_unlock(&android_app->mutex);
    // Can't touch android_app object after this.
}

// Fix per Google for bug https://code.google.com/p/android/issues/detail?id=41755
static void process_input(struct android_app* app, struct android_poll_source* source) {
    AInputEvent* event = NULL;
    int processed = 0;
    while (AInputQueue_getEvent(app->inputQueue, &event) >= 0) {
//        LOGV("New input event: type=%d\n", AInputEvent_getType(event));
        if (AInputQueue_preDispatchEvent(app->inputQueue, event)) {
            continue;
        }
        int32_t handled = 0;
        if (app->onInputEvent != NULL) handled = app->onInputEvent(app, event);
        AInputQueue_finishEvent(app->inputQueue, event, handled);
        processed = 1;
    }
    if (processed == 0) {
        LOGE("Failure reading next input event: %s\n", strerror(errno));
    }
}

static void process_cmd(struct android_app* app, struct android_poll_source* source) {
    int8_t cmd = android_app_read_cmd(app);
    android_app_pre_exec_cmd(app, cmd);
    if (app->onAppCmd != NULL) app->onAppCmd(app, cmd);
    android_app_post_exec_cmd(app, cmd);
}

static void* android_app_entry(void* param) {
    struct android_app* android_app = (struct android_app*)param;

    android_app->config = AConfiguration_new();
    AConfiguration_fromAssetManager(android_app->config, android_app->activity->assetManager);

    print_cur_config(android_app);

    android_app->cmdPollSource.id = LOOPER_ID_MAIN;
    android_app->cmdPollSource.app = android_app;
    android_app->cmdPollSource.process = process_cmd;
    android_app->inputPollSource.id = LOOPER_ID_INPUT;
    android_app->inputPollSource.app = android_app;
    android_app->inputPollSource.process = process_input;

    ALooper* looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd(looper, android_app->msgread, LOOPER_ID_MAIN, ALOOPER_EVENT_INPUT, NULL,
            &android_app->cmdPollSource);
    android_app->looper = looper;

	int error = (*android_app->activity->vm)->AttachCurrentThread(android_app->activity->vm, 
		&android_app->appThreadEnv, NULL);
	if (error)
		android_app->appThreadEnv = NULL;

    pthread_mutex_lock(&android_app->mutex);
    android_app->running = 1;
    pthread_cond_broadcast(&android_app->cond);
    pthread_mutex_unlock(&android_app->mutex);

    android_main(android_app);

	(*android_app->activity->vm)->DetachCurrentThread(android_app->activity->vm);

    android_app_destroy(android_app);

	return NULL;
}

// --------------------------------------------------------------------
// Native activity interaction (called from main thread)
// --------------------------------------------------------------------

static struct android_app* android_app_create(ANativeActivity* activity,
        void* savedState, size_t savedStateSize) {
    struct android_app* android_app = (struct android_app*)malloc(sizeof(struct android_app));
    memset(android_app, 0, sizeof(struct android_app));
    android_app->activity = activity;

    pthread_mutex_init(&android_app->mutex, NULL);
    pthread_cond_init(&android_app->cond, NULL);

    if (savedState != NULL) {
        android_app->savedState = malloc(savedStateSize);
        android_app->savedStateSize = savedStateSize;
        memcpy(android_app->savedState, savedState, savedStateSize);
    }

    int msgpipe[2];
    if (pipe(msgpipe)) {
        LOGI("could not create pipe: %s", strerror(errno));
    }
    android_app->msgread = msgpipe[0];
    android_app->msgwrite = msgpipe[1];

    android_app->appThreadThis = (*activity->env)->NewGlobalRef(activity->env, activity->clazz);

	jclass contextClass = (*activity->env)->FindClass(activity->env, "android/content/Context");
	jclass winManagerClass = (*activity->env)->FindClass(activity->env, "android/view/WindowManager");
	jclass displayClass = (*activity->env)->FindClass(activity->env, "android/view/Display");

	jfieldID WINDOW_SERVICE_id = (*activity->env)->GetStaticFieldID(activity->env, 
		contextClass, "WINDOW_SERVICE", "Ljava/lang/String;");

	jobject WINDOW_SERVICE = (*activity->env)->GetStaticObjectField(activity->env, 
		contextClass, WINDOW_SERVICE_id);

	jmethodID getSystemService = (*activity->env)->GetMethodID(activity->env, contextClass, 
		"getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

	jobject winManager = (*activity->env)->CallObjectMethod(activity->env, activity->clazz,
		getSystemService, WINDOW_SERVICE);

	jmethodID getDefaultDisplay = (*activity->env)->GetMethodID(activity->env, winManagerClass, 
		"getDefaultDisplay", "()Landroid/view/Display;");

	android_app->appThreadDisp = (*activity->env)->CallObjectMethod(activity->env, winManager,
		getDefaultDisplay);

	android_app->appThreadDisp = (*activity->env)->NewGlobalRef(activity->env, android_app->appThreadDisp);

	android_app->appThreadDispGetRotation = (*activity->env)->GetMethodID(activity->env, displayClass, 
		"getRotation", "()I");

	if (nv_android_init)
		nv_android_init(activity);

    pthread_attr_t attr; 
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&android_app->thread, &attr, android_app_entry, android_app);

    // Wait for thread to start.
    pthread_mutex_lock(&android_app->mutex);
    while (!android_app->running) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }
    pthread_mutex_unlock(&android_app->mutex);

    return android_app;
}

static void android_app_write_cmd(struct android_app* android_app, int8_t cmd) {
    if (write(android_app->msgwrite, &cmd, sizeof(cmd)) != sizeof(cmd)) {
        LOGI("Failure writing android_app cmd: %s\n", strerror(errno));
    }
}

static void android_app_set_input(struct android_app* android_app, AInputQueue* inputQueue) {
    pthread_mutex_lock(&android_app->mutex);
    android_app->pendingInputQueue = inputQueue;
    android_app_write_cmd(android_app, APP_CMD_INPUT_CHANGED);
    while (android_app->inputQueue != android_app->pendingInputQueue) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }
    pthread_mutex_unlock(&android_app->mutex);
}

static void android_app_set_window(struct android_app* android_app, ANativeWindow* window) {
    pthread_mutex_lock(&android_app->mutex);
    if (android_app->pendingWindow != NULL) {
        android_app_write_cmd(android_app, APP_CMD_TERM_WINDOW);
    }
    android_app->pendingWindow = window;
    if (window != NULL) {
        android_app_write_cmd(android_app, APP_CMD_INIT_WINDOW);
    }
    while (android_app->window != android_app->pendingWindow) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }
    pthread_mutex_unlock(&android_app->mutex);
}

static void nv_app_resize_window(struct android_app* android_app, ANativeWindow* window) {
    android_app_write_cmd(android_app, APP_CMD_WINDOW_RESIZED);
}

static void android_app_set_activity_state(struct android_app* android_app, int8_t cmd) {
    pthread_mutex_lock(&android_app->mutex);
    android_app_write_cmd(android_app, cmd);
    while (android_app->activityState != cmd) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }
    pthread_mutex_unlock(&android_app->mutex);
}

static void android_app_free(struct android_app* android_app) {
    pthread_mutex_lock(&android_app->mutex);
    android_app_write_cmd(android_app, APP_CMD_DESTROY);
    while (!android_app->destroyed) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }
    pthread_mutex_unlock(&android_app->mutex);

	(*android_app->activity->env)->DeleteGlobalRef(android_app->activity->env,
		android_app->appThreadThis);
	(*android_app->activity->env)->DeleteGlobalRef(android_app->activity->env,
		android_app->appThreadDisp);

    close(android_app->msgread);
    close(android_app->msgwrite);
    pthread_cond_destroy(&android_app->cond);
    pthread_mutex_destroy(&android_app->mutex);
    free(android_app);
}

static void onDestroy(ANativeActivity* activity) {
    LOGI("Destroy: %p\n", activity);
    android_app_free((struct android_app*)activity->instance);
}

static void onStart(ANativeActivity* activity) {
    LOGI("Start: %p\n", activity);
    android_app_set_activity_state((struct android_app*)activity->instance, APP_CMD_START);
}

static void onResume(ANativeActivity* activity) {
    LOGI("Resume: %p\n", activity);
    android_app_set_activity_state((struct android_app*)activity->instance, APP_CMD_RESUME);
}

static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    void* savedState = NULL;

    LOGI("SaveInstanceState: %p\n", activity);
    pthread_mutex_lock(&android_app->mutex);
    android_app->stateSaved = 0;
    android_app_write_cmd(android_app, APP_CMD_SAVE_STATE);
    while (!android_app->stateSaved) {
        pthread_cond_wait(&android_app->cond, &android_app->mutex);
    }

    if (android_app->savedState != NULL) {
        savedState = android_app->savedState;
        *outLen = android_app->savedStateSize;
        android_app->savedState = NULL;
        android_app->savedStateSize = 0;
    }

    pthread_mutex_unlock(&android_app->mutex);

    return savedState;
}

static void onPause(ANativeActivity* activity) {
    LOGI("Pause: %p\n", activity);
    android_app_set_activity_state((struct android_app*)activity->instance, APP_CMD_PAUSE);
}

static void onStop(ANativeActivity* activity) {
    LOGI("Stop: %p\n", activity);
    android_app_set_activity_state((struct android_app*)activity->instance, APP_CMD_STOP);
}

static void onConfigurationChanged(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    LOGI("ConfigurationChanged: %p\n", activity);
    android_app_write_cmd(android_app, APP_CMD_CONFIG_CHANGED);
}

static void onLowMemory(ANativeActivity* activity) {
    struct android_app* android_app = (struct android_app*)activity->instance;
    LOGI("LowMemory: %p\n", activity);
    android_app_write_cmd(android_app, APP_CMD_LOW_MEMORY);
}

static void onWindowFocusChanged(ANativeActivity* activity, int focused) {
    LOGI("WindowFocusChanged: %p -- %d\n", activity, focused);
    android_app_write_cmd((struct android_app*)activity->instance,
            focused ? APP_CMD_GAINED_FOCUS : APP_CMD_LOST_FOCUS);
}

static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("NativeWindowCreated: %p -- %p\n", activity, window);
    android_app_set_window((struct android_app*)activity->instance, window);
}

static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("NativeWindowResized: %p -- %p\n", activity, window);
    nv_app_resize_window((struct android_app*)activity->instance, window);
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("NativeWindowDestroyed: %p -- %p\n", activity, window);
    android_app_set_window((struct android_app*)activity->instance, NULL);
}

static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
    LOGI("InputQueueCreated: %p -- %p\n", activity, queue);
    android_app_set_input((struct android_app*)activity->instance, queue);
}

static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
    LOGI("InputQueueDestroyed: %p -- %p\n", activity, queue);
    android_app_set_input((struct android_app*)activity->instance, NULL);
}

void ANativeActivity_onCreate(ANativeActivity* activity,
        void* savedState, size_t savedStateSize) {
    LOGI("Creating: %p\n", activity);
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
	activity->callbacks->onNativeWindowResized = onNativeWindowResized;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;

    struct android_app* inst = android_app_create(activity, savedState, savedStateSize);

	inst->lifecycleFlags |= NV_APP_STATUS_RUNNING;

	activity->instance = inst;
}


// NV extensions to return the current status of the app's activity
// Between onCreate and onDestroy, returns nonzero.  Else returns 0
int nv_app_status_running(struct android_app* android_app)
{
	return (android_app->lifecycleFlags & NV_APP_STATUS_RUNNING) ? 1 : 0;
}

// Between onResume and onPause, returns nonzero.  Else returns 0
int nv_app_status_active(struct android_app* android_app)
{
	return (android_app->lifecycleFlags & NV_APP_STATUS_ACTIVE) ? 1 : 0;
}

// Between onFocusChanged(true) and onFocusChanged(false), returns nonzero.  Else returns 0
int nv_app_status_focused(struct android_app* android_app)
{
	return (android_app->lifecycleFlags & NV_APP_STATUS_FOCUSED) ? 1 : 0;
}

// If there is a current Android surface (not EGLSurface) 
// and it has nonzero width and height, returns nonzero.  Else returns 0
int nv_app_status_valid_surface(struct android_app* android_app)
{
	return (android_app->lifecycleFlags & NV_APP_STATUS_HAS_REAL_SURFACE) ? 1 : 0;
}

// Logical AND of nv_app_status_running, _active, _focused and _valid_surface
int nv_app_status_interactable(struct android_app* android_app)
{
    return ((android_app->lifecycleFlags & NV_APP_STATUS_INTERACTABLE) 
		== NV_APP_STATUS_INTERACTABLE) ? 1 : 0; 
}

int nv_app_get_display_rotation(struct android_app* android_app)
{
	return (*android_app->appThreadEnv)->CallIntMethod(android_app->appThreadEnv, 
		android_app->appThreadDisp, android_app->appThreadDispGetRotation);
}

/*
    Force exit and wait for the events to work themselves out
*/
void nv_app_force_quit_no_cleanup(struct android_app* android_app)
{
    ANativeActivity_finish(android_app->activity);

    /* 
        we need to process all of the messages waiting for exit.
        This will cause trouble if the app has its own processor added
        We need the default _only_ here, so we null out the app's handlers (if any)
    */
    android_app->onAppCmd = NULL;
    android_app->onInputEvent = NULL;
    while (nv_app_status_running(android_app))
    {
        int events;
        struct android_poll_source* source;
        if (ALooper_pollAll(-1, NULL, &events, (void**)&source) < 0)
            return;
        if (source != NULL)
            source->process(android_app, source);
    }
}
