/*
* Timer based pump control.
* Author : Subhajit Roy
* subhajitroy005@gmail.com
* TAB size 2
*/



/****************************************************
 * 
 * Architecture specific macros 
 *
 ***************************************************/
#define PUMP_CTRL_MODEL_1     1

#define SYSTEM_VARIANCE       PUMP_CTRL_MODEL_1


#define DEBUG_EN              0
#define EEPROM_ADDRESS        0


/****************************************************
 * 
 * Common includes
 *
 ***************************************************/
#include <stdint.h>

/* Devive specific include */
#if( SYSTEM_VARIANCE == PUMP_CTRL_MODEL_1)
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  #include <EEPROM.h>
#endif 










/* Syatem Constrains */

#define MAX_TIMER_OPERATION_IN_MIN  600
#define MIN_TIMER_OPERATION_IN_MIN  1



/****************************************************
 * 
 * Global variables and objects
 *
 ***************************************************/
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display



typedef struct _timer_information
{
    uint32_t       user_timer_setup;  // Running time set by user
    uint64_t       operation_start_timestamp;
    uint32_t       running_timer_status;
    uint32_t       ui_refresh_ts; // for UI refresh purpose    
    uint32_t       minute_display_scaling_factor_hold;
}timer_info;

typedef struct _ui_information
{
    uint8_t       notification_flag;  // Running time set by user
    
}ui_info;

/* 
 * State machine related variables 
 */
typedef enum state_list
{
  STATE_TIMER_SETUP = 0, 
  STATE_IDLE,
  STATE_RUNNING
};

enum signals_list{
  SIGNAL_START = 0,
  SIGNAL_UP,
  SIGNAL_DOWN,
  SIGNAL_OK,
  SIGNAL_SET,
  SIGNAL_STOP
};

struct application_info
{
    uint8_t        next_state;  // Hold the state for next state transition
    uint8_t        curr_state;  // Hold the state from where the next state is transiting
    uint8_t        cap_signal;  // captured signal

    
    uint8_t        op_mode;     // Operation modes select by user

    uint8_t        env_condition;

    
    // ---  Timer related info
    timer_info     timer;       // timer

    // --- UI Info
    ui_info        ui;
}application;



/****************************************************
 * 
 * Input and output related sections
 *
 ***************************************************/
/* State related variables */
#define MAX_IO_SCAN_PINS          5             // maximum scan pins for user input
#define MAX_OUT_PINS              1             // Maximum output pins 

/* 
 * OUTPUT PINS declarations
 */
#define PIN_RELAY                 4  //  PD4

/* 
 * INPUT PINS declarations
 */
#define PIN_START                 7   // PD7
#define PIN_UP                    10  // PB2
#define PIN_DOWN                  8   // PB0
#define PIN_OK                    6   // PB1
#define PIN_SET                   9   // PD6


/* 
 * Logic States
 */
#define BOTH_STATE           2
#define CONDITION_REGISTERED true
#define CONDITION_SERVED     false

#define PUMP_ON_STATE        true
#define PUMP_OFF_STATE       false

enum  scan_pin_index_poll
{                  
  INDEX_SCAN_PIN_START = 0,
  INDEX_SCAN_PIN_UP,
  INDEX_SCAN_PIN_DOWN,
  INDEX_SCAN_PIN_OK ,
  INDEX_SCPN_PIN_SET 
};

enum out_pin_index
{                         // Output pin indexes for driver purpose
  INDEX_OUT_PIN_RELAY = 0,
};

volatile struct _scan_pins
{
    uint8_t        pin;
    uint8_t        pin_state;            // Current state at polling
    uint8_t        previous_state;       // Debounce purpose comparison purpose
    uint8_t        present_state;        // Reflect the hold state
    uint32_t       debounce_time;
    uint32_t       last_debounce_time;   // State change debounce time capture
    uint8_t        pressed_state;        // Configuration based on circuit design high/low
    uint8_t        is_pressed;           // Flag for resiater a signa for button is pressed
}scan_pin[MAX_IO_SCAN_PINS];
        
volatile struct _output_pins
{
    uint8_t        pin;
    uint32_t       pin_state;
    
}output_pin[MAX_OUT_PINS];




volatile uint32_t pump_operation_time_min = 0; // in minutes 
volatile uint32_t pump_operation_start_time = 0; // in minutes 
volatile uint32_t ui_operation_update_time = 0; // in minutes 
volatile uint32_t pump_current_time_update = 0; // in minutes 
