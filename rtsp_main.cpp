#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <QMainWindow>
#include <QAction>
#include <mutex>
#include <net/Logger.h>
#include "helper.h"
#include "rtsp_output_helper.h"
#include "rtsp_output.h"
#include "ui/rtsp_properties.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-rtspserver", "en-US")

void obs_frontend_event(enum obs_frontend_event event, void *ptr);
void rtsp_output_auto_start(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_stop(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_save_settings(RtspOutputHelper *rtspOutputHelper);
void rtsp_output_save_hotkey_settings(RtspOutputHelper *rtspOutputHelper);
void server_log_write_callback(xop::Priority priority, std::string info);
void rtsp_menu_callback(void *private_data);

// 全局变量存储输出助手和UI组件
static RtspOutputHelper *rtsp_output_helper = nullptr;
static RtspProperties *rtsp_properties = nullptr;

const char *obs_module_name(void)
{
	return obs_module_text("RtspServer");
}

const char *obs_module_description(void)
{
	return obs_module_text("RstpServer.Description");
}

bool obs_module_load(void)
{
	xop::Logger::Instance().SetWriteCallback(server_log_write_callback);
	rtsp_output_register();

	// 延迟所有初始化到OBS完全加载后
	obs_frontend_add_event_callback(obs_frontend_event, nullptr);

	return true;
}

void obs_module_unload(void)
{
	obs_frontend_remove_event_callback(obs_frontend_event, nullptr);
}

void obs_frontend_event(enum obs_frontend_event event, void *)
{
	switch (event) {
	case OBS_FRONTEND_EVENT_FINISHED_LOADING:
		// 现在OBS已完全加载，可以安全创建RTSP输出对象
		if (!rtsp_output_helper) {
			// 创建RtspOutputHelper
			auto *settings = rtsp_output_read_data();
			auto *config = rtsp_properties_open_config();
			const char *str = nullptr;
			str = config_get_string(config, HOTKEY_CONFIG_SECTIION, "RtspOutput");
			obs_data_t *hotkey = nullptr;
			if (str) hotkey = obs_data_create_from_json(str);
			rtsp_output_helper = RtspOutputHelper::CreateRtspOutput(settings, hotkey);
			obs_data_release(hotkey);
			config_close(config);
			obs_data_release(settings);

			// 使用回调方式创建菜单项，避免Qt初始化问题
			obs_frontend_add_tools_menu_item(obs_module_text("RtspServer"), 
				rtsp_menu_callback, rtsp_output_helper);
		}
		rtsp_output_auto_start(rtsp_output_helper);
		break;
	case OBS_FRONTEND_EVENT_EXIT:
		if (rtsp_output_helper) {
			rtsp_output_stop(rtsp_output_helper);
			rtsp_output_save_settings(rtsp_output_helper);
			rtsp_output_save_hotkey_settings(rtsp_output_helper);
			delete rtsp_output_helper;
			rtsp_output_helper = nullptr;
		}
		break;
	default: ;
	}
}

void rtsp_output_auto_start(RtspOutputHelper *rtspOutputHelper)
{
	auto *config = rtsp_properties_open_config();
	auto autoStart = false;
	if (config) {
		autoStart =
			config_get_bool(config, CONFIG_SECTIION, "AutoStart");
		config_close(config);
	}
	if (autoStart)
		rtspOutputHelper->Start();
}

void rtsp_output_stop(RtspOutputHelper *rtspOutputHelper)
{
	rtspOutputHelper->Stop();
}

void rtsp_output_save_hotkey_settings(RtspOutputHelper *rtspOutputHelper)
{
	auto *data = rtspOutputHelper->HotkeysSave();
	auto *str = obs_data_get_json(data);
	auto *config = rtsp_properties_open_config();
	config_set_string(config, HOTKEY_CONFIG_SECTIION, "RtspOutput", str);
	config_save(config);
	config_close(config);
}

void rtsp_output_save_settings(RtspOutputHelper *rtspOutputHelper)
{
	auto *data = rtspOutputHelper->GetSettings();
	rtsp_output_save_data(data);
}

void server_log_write_callback(xop::Priority priority, std::string info)
{
	switch (priority) {
	case xop::LOG_DEBUG:
		blog(LOG_DEBUG, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_STATE:
		blog(LOG_INFO, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_INFO:
		blog(LOG_INFO, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_WARNING:
		blog(LOG_WARNING, "[rtsp-server] %s", info.c_str());
		break;
	case xop::LOG_ERROR:
		blog(LOG_ERROR, "[rtsp-server] %s", info.c_str());
		break;
	default: ;
	}
}

void rtsp_menu_callback(void *private_data)
{
	auto *rtspOutputHelper = static_cast<RtspOutputHelper *>(private_data);
	
	blog(LOG_INFO, "[rtsp-server] Menu callback triggered");
	
	if (!rtspOutputHelper) {
		blog(LOG_ERROR, "[rtsp-server] RtspOutputHelper is null");
		return;
	}
	
	// 延迟创建Qt对话框，只在需要时创建
	if (!rtsp_properties) {
		blog(LOG_INFO, "[rtsp-server] Creating RTSP properties dialog");
		
		const auto mainWindow = static_cast<QMainWindow *>(obs_frontend_get_main_window());
		if (!mainWindow) {
			blog(LOG_ERROR, "[rtsp-server] Failed to get main window");
			return;
		}
		
		try {
			blog(LOG_INFO, "[rtsp-server] About to create QDialog test");
			
			// 先测试创建一个简单的QDialog
			auto *testDialog = new QDialog(mainWindow);
			testDialog->setWindowTitle("RTSP Server Test");
			testDialog->resize(300, 200);
			blog(LOG_INFO, "[rtsp-server] Simple QDialog created successfully");
			
			// 如果简单对话框成功，再尝试创建完整的属性对话框
			obs_frontend_push_ui_translation(obs_module_get_string);
			rtsp_properties = new RtspProperties(rtspOutputHelper, mainWindow);
			obs_frontend_pop_ui_translation();
			blog(LOG_INFO, "[rtsp-server] RTSP properties dialog created successfully");
			
			// 删除测试对话框
			delete testDialog;
		} catch (const std::exception& e) {
			obs_frontend_pop_ui_translation();
			blog(LOG_ERROR, "[rtsp-server] Failed to create RTSP properties dialog: %s", e.what());
			return;
		} catch (...) {
			obs_frontend_pop_ui_translation();
			blog(LOG_ERROR, "[rtsp-server] Failed to create RTSP properties dialog: unknown error");
			return;
		}
	}
	
	if (rtsp_properties) {
		blog(LOG_INFO, "[rtsp-server] Showing RTSP properties dialog");
		try {
			rtsp_properties->exec();
		} catch (const std::exception& e) {
			blog(LOG_ERROR, "[rtsp-server] Failed to show RTSP properties dialog: %s", e.what());
		} catch (...) {
			blog(LOG_ERROR, "[rtsp-server] Failed to show RTSP properties dialog: unknown error");
		}
	}
}
