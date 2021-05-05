/****************************************************************************
 *   Aug 3 12:17:11 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "config.h"
#include <TTGO.h>
#include <WiFi.h>
#include <oc_api.h>
#include <oc_pki.h>
#include <oc_core_res.h>



#include "esp_event.h"
#include "esp_wifi.h"

#include "ocf_app.h"
#include "ocf_app_main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_styles.h"
#include "images/bulb.c"

#define STACK_SIZE 20000
static EventGroupHandle_t wifi_event_group;

static const char *device_name = "T-Watch";
static const char *TAG = "iotivity server";


static void
set_device_custom_property(void *data)
{
  (void)data;
  oc_set_custom_device_property(purpose, "desk lamp");
}

static int app_init(void){
    int ret = oc_init_platform("Intel",NULL,NULL);
    ret != oc_add_device("/oic/d", "oic.d.light", device_name, "ocf.1.0.0", "ocf.res.1.0.0", set_device_custom_property, NULL);
}




void factory_presets_cb_new(size_t device, void *data)
{
  oc_device_info_t *dev = oc_core_get_device_info(device);
  oc_free_string(&dev->name);
  oc_new_string(&dev->name, device_name, strlen(device_name));
  (void)data;
#if defined(OC_SECURITY) && defined(OC_PKI)
  PRINT("factory_presets_cb: %d\n", (int)device);
#endif /* OC_SECURITY && OC_PKI */
}

static void oc_server_main(void *pvParameter)
{
    int init;
    struct ip6_addr if_ipaddr_ip6 = {0};
    log_i("Started OCF Server");
    while (1){
        if (tcpip_adapter_get_ip6_linklocal(TCPIP_ADAPTER_IF_STA, &if_ipaddr_ip6) != ESP_OK)
        {
            log_e("get IPv6 address failed");
        }
        else
        {
            log_i("got IPv6 addr:%s", ip6addr_ntoa(&if_ipaddr_ip6));
        }
        //Loop forever tasks do not return
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}


lv_obj_t *ocf_app_main_tile = NULL;
lv_style_t ocf_app_main_style;

lv_task_t * _ocf_app_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_ocf_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_ocf_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void ocf_app_task( lv_task_t * task );

void ocf_app_main_setup( uint32_t tile_num ) {

    ocf_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &ocf_app_main_style, ws_get_mainbar_style() );

    
    lv_obj_t * on_img = lv_img_create(ocf_app_main_tile,NULL);
    lv_img_set_src(on_img, &bulb);
    lv_obj_align(on_img,NULL,LV_ALIGN_CENTER,0,0);

    lv_obj_t * exit_btn = lv_imgbtn_create( ocf_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &ocf_app_main_style );
    lv_obj_align(exit_btn, ocf_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_ocf_app_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( ocf_app_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &ocf_app_main_style );
    lv_obj_align(setup_btn, ocf_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_ocf_app_setup_event_cb );

    // uncomment the following block of code to remove the "myapp" label in background
    /*lv_style_set_text_opa( &ocf_app_main_style, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font( &ocf_app_main_style, LV_STATE_DEFAULT, &Ubuntu_72px);
    lv_obj_t *app_label = lv_label_create( ocf_app_main_tile, NULL);
    lv_label_set_text( app_label, "OCF");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &ocf_app_main_style );
    lv_obj_align( app_label, ocf_app_main_tile, LV_ALIGN_CENTER, 0, 0);
    */
    
    static StackType_t xStack[STACK_SIZE];
    TaskHandle_t xHandle = NULL;
    // Structure that will hold the TCB of the task being created.
    static StaticTask_t xTaskBuffer;
    BaseType_t xReturned;
     xReturned = xTaskCreate(
      oc_server_main,   // Function that implements the task.
      "server_main", // Text name for the task.
      STACK_SIZE,    // Stack size in bytes, not words.
      NULL,          // Parameter passed into the task.
      5,             // Priority at which the task is created.
      &xHandle
      ); // Variable to hold the task's data structure.

    // create an task that runs every secound
    _ocf_app_task = lv_task_create( ocf_app_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static void enter_ocf_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( ocf_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_ocf_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void ocf_app_task( lv_task_t * task ) {
    // put your code her
    //const char *ipaddress = WiFi.localIP().toString().c_str();
    //log_i("OCF App is active: IP %s",ipaddress);
}