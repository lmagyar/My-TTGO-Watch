/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#include "screenshot.h"

#include "utils/alloc.h"

static uint8_t *png;

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

void screenshot_setup( void ) {
    png = (uint8_t*)MALLOC( lv_disp_get_hor_res( NULL ) * lv_disp_get_ver_res( NULL ) * sizeof( lv_color_t ) );
    if ( png == NULL ) {
        log_e("screenshot malloc failed");
        while(1);
    }
}

void screenshot_take( void ) {
    lv_disp_drv_t driver;
    lv_disp_t *system_disp;

    log_i("take screenshot");
    
    system_disp = lv_disp_get_default();
    driver.flush_cb = system_disp->driver.flush_cb;
    system_disp->driver.flush_cb = screenshot_disp_flush;
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now( system_disp );
    system_disp->driver.flush_cb = driver.flush_cb;
}

void screenshot_save( void ) {
    uint8_t *data_p = png;

    remove( SCREENSHOT_FILE_NAME );
    FILE *file = fopen( SCREENSHOT_FILE_NAME, "wb" );

    if ( file ) {
        fwrite( (const void*)data_p, 1, 240 * 240 *2, file );
        log_i("save screenshot");
    }
    else {
        log_e("error while open file");
    }
    fflush( file );
    fclose( file );
}

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p ) {

    uint32_t x, y;
    uint16_t *data = (uint16_t *)color_p;

    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            *(png + (y * lv_disp_get_hor_res( NULL ) + x )) = *data;
            data++;
        }
    } 
    lv_disp_flush_ready(disp_drv);
}
