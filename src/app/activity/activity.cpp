#include "config.h"
#include <TTGO.h>
#include "quickglui/quickglui.h"

#include "activity.h"
#include "gui/mainbar/mainbar.h"
#include "hardware/bma.h"

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(move_64px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

static Application activityApp;
static JsonConfig config("activity.json");

// Options
static String size, length, goal_step, goal_dist;
// Widgets
static Label lblStepcounter, lblStepachievement;
static Label lblDistance, lblDistachievement;
static Arc arcStepcounter, arcDistance;

static Style big, small;


static void build_main_page();
static void refresh_main_page();
static void build_settings();

static void activity_activate_cb();

/*
 * setup routine for application
 */
void activity_app_setup() {
    // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    activityApp.init("activity", &move_64px, 1, 1);

    mainbar_add_tile_activate_cb( activityApp.mainTileId(), activity_activate_cb );

    // Build and configure application
    build_main_page();
    build_settings();

    refresh_main_page();
}

void build_main_page()
{
    big = Style::Create(mainbar_get_style(), true);
    big.textFont(&Ubuntu_32px)
      .textOpacity(LV_OPA_80);
    small = Style::Create(mainbar_get_style(), true);
    small.textFont(&Ubuntu_16px)
      .textOpacity(LV_OPA_80);

    AppPage& screen = activityApp.mainPage(); // This is parent for all main screen widgets

    arcStepcounter = Arc(&screen, 0, 360);
    arcStepcounter.start(0).end(0).rotation(90)
        .style(mainbar_get_arc_style(), LV_ARC_PART_INDIC, false )
        .style(mainbar_get_arc_bg_style(), LV_ARC_PART_BG, false )
        .size(120, 120)
        .alignInParentBottomLeft(0, -42);

    lblStepcounter = Label(&screen);
    lblStepcounter.text("0")
        .style(big, true)
        .align(arcStepcounter, LV_ALIGN_OUT_TOP_MID);
    
    lblStepachievement = Label(&screen);
    lblStepachievement.text("0%")
        .style(small, true)
        .alignOrig0(arcStepcounter, LV_ALIGN_CENTER);
    
    arcDistance = Arc(&screen, 0, 360);
    arcDistance.start(0).end(0).rotation(90)
        .style(mainbar_get_arc_style(), LV_ARC_PART_INDIC, false )
        .style(mainbar_get_arc_bg_style(), LV_ARC_PART_BG, false )
        .size(120, 120)
        .alignInParentBottomRight(0, -42);

    lblDistance = Label(&screen);
    lblDistance.text("0")
        .style(big, true)
        .align(arcDistance, LV_ALIGN_OUT_TOP_MID);
    
    lblDistachievement = Label(&screen);
    lblDistachievement.text("0")
        .style(small, true)
        .alignOrig0(arcDistance, LV_ALIGN_CENTER);
}

void refresh_main_page()
{
    char buff[36];
    uint32_t gStep = goal_step.toInt();
    uint32_t gDist = goal_dist.toInt();
    // Get current value
    uint32_t stp = bma_get_stepcounter();
    uint32_t ach = gStep == 0 ? 0 : 100 * stp / gStep;
    uint32_t dist = stp * length.toInt() / 100;
    log_d("Refresh activity: %d steps", stp);
    // Raw steps
    snprintf( buff, sizeof( buff ), "%d", stp );
    lblStepcounter.text(buff).realign();
    // Achievement
    snprintf( buff, sizeof( buff ), "%d%%", ach );
    lblStepachievement.text(buff).realign();
    arcStepcounter.end( gStep == 0 ? 0 : 360 * stp / gStep );
    // Distance
    snprintf( buff, sizeof( buff ), "%d m", dist );
    lblDistance.text(buff).realign();
    // Achievement
    snprintf( buff, sizeof( buff ), "%d%%", gDist == 0 ? 0 : 100 * dist / gDist );
    lblDistachievement.text(buff).realign();
    arcDistance.end( gDist == 0 ? 0 : 360 * dist / gDist );
}

void activity_activate_cb()
{
    refresh_main_page();
}

void build_settings()
{
    // Create full options list and attach items to variables
    config.addString("Step length (cm)", 3, "50").setDigitsMode(true,"0123456789").assign(&length); // cm
    config.addString("Step Goal", 7, "10000").setDigitsMode(true,"0123456789").assign(&goal_step); // steps
    config.addString("Distance Goal", 7, "5000").setDigitsMode(true,"0123456789").assign(&goal_dist); // m

    activityApp.useConfig(config, true); // true - auto create settings page widgets
}
