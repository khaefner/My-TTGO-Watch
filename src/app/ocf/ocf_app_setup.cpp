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

#include "ocf_app.h"
#include "ocf_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"



lv_obj_t *ocf_app_setup_tile = NULL;
lv_style_t ocf_app_setup_style;

lv_obj_t *ocf_app_foobar_switch = NULL;

LV_IMG_DECLARE(exit_32px);

static void exit_ocf_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void ocf_app_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event );

void ocf_app_setup_setup( uint32_t tile_num ) {
    log_i("Called OCF App");
    ocf_app_setup_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &ocf_app_setup_style, ws_get_setup_tile_style() );
    lv_obj_add_style( ocf_app_setup_tile, LV_OBJ_PART_MAIN, &ocf_app_setup_style );

    lv_obj_t *exit_cont = lv_obj_create( ocf_app_setup_tile, NULL );
    lv_obj_set_size( exit_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( exit_cont, LV_OBJ_PART_MAIN, &ocf_app_setup_style  );
    lv_obj_align( exit_cont, ocf_app_setup_tile, LV_ALIGN_IN_TOP_MID, 0, 10 );

    lv_obj_t *exit_btn = lv_imgbtn_create( exit_cont, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &ocf_app_setup_style );
    lv_obj_align( exit_btn, exit_cont, LV_ALIGN_IN_TOP_LEFT, 10, 0 );
    lv_obj_set_event_cb( exit_btn, exit_ocf_app_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( exit_cont, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &ocf_app_setup_style  );
    lv_label_set_text( exit_label, "my app setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *ocf_app_foobar_switch_cont = lv_obj_create( ocf_app_setup_tile, NULL );
    lv_obj_set_size( ocf_app_foobar_switch_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( ocf_app_foobar_switch_cont, LV_OBJ_PART_MAIN, &ocf_app_setup_style  );
    lv_obj_align( ocf_app_foobar_switch_cont, exit_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );

    ocf_app_foobar_switch = wf_add_switch( ocf_app_foobar_switch_cont, false );
    lv_obj_align( ocf_app_foobar_switch, ocf_app_foobar_switch_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( ocf_app_foobar_switch, ocf_app_foobar_switch_event_cb );

    lv_obj_t *ocf_app_foobar_switch_label = lv_label_create( ocf_app_foobar_switch_cont, NULL);
    lv_obj_add_style( ocf_app_foobar_switch_label, LV_OBJ_PART_MAIN, &ocf_app_setup_style  );
    lv_label_set_text( ocf_app_foobar_switch_label, "foo bar");
    lv_obj_align( ocf_app_foobar_switch_label, ocf_app_foobar_switch_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
}

static void ocf_app_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): Serial.printf( "switch value = %d\r\n", lv_switch_get_state( obj ) );
                                        break;
    }
}

static void exit_ocf_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( ocf_app_get_app_main_tile_num(), LV_ANIM_ON );
                                        break;
    }
}
