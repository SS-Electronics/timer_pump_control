
/**********************************************************
 * 
 * function:  io init
 * parent state call: main setup
 * 
***********************************************************/

void func_io_driver_int(struct application_info* app)
{
    /* 
     * OutPut pin Inits
     */
    scan_pin[INDEX_SCAN_PIN_START].pin            = PIN_START;
    scan_pin[INDEX_SCAN_PIN_START].pressed_state  = LOW;
    scan_pin[INDEX_SCAN_PIN_START].debounce_time  = 30; // Debounce time in 30mS
    pinMode(scan_pin[INDEX_SCAN_PIN_START].pin , INPUT_PULLUP);
    // When the system is starting capure the state 
    scan_pin[INDEX_SCAN_PIN_START].previous_state = digitalRead(scan_pin[INDEX_SCAN_PIN_START].pin );


    scan_pin[INDEX_SCAN_PIN_UP].pin               = PIN_UP;
    scan_pin[INDEX_SCAN_PIN_UP].pressed_state     = LOW;
    scan_pin[INDEX_SCAN_PIN_UP].debounce_time     = 30; // Debounce time in 30mS
    pinMode(scan_pin[INDEX_SCAN_PIN_UP].pin , INPUT_PULLUP);
    // When the system is starting capure the state 
    scan_pin[INDEX_SCAN_PIN_UP].previous_state    = digitalRead(scan_pin[INDEX_SCAN_PIN_UP].pin );


    scan_pin[INDEX_SCAN_PIN_DOWN].pin             = PIN_DOWN;
    scan_pin[INDEX_SCAN_PIN_DOWN].pressed_state   = LOW;
    scan_pin[INDEX_SCAN_PIN_DOWN].debounce_time   = 30; // Debounce time in 30mS
    pinMode(scan_pin[INDEX_SCAN_PIN_DOWN].pin , INPUT_PULLUP);
    // When the system is starting capure the state 
    scan_pin[INDEX_SCAN_PIN_DOWN].previous_state  = digitalRead(scan_pin[INDEX_SCAN_PIN_DOWN].pin );


    scan_pin[INDEX_SCPN_PIN_SET].pin             = PIN_SET;
    scan_pin[INDEX_SCPN_PIN_SET].pressed_state   = LOW;
    scan_pin[INDEX_SCPN_PIN_SET].debounce_time   = 30; // Debounce time in 30mS
    pinMode(scan_pin[INDEX_SCPN_PIN_SET].pin , INPUT_PULLUP);
    // When the system is starting capure the state 
    scan_pin[INDEX_SCPN_PIN_SET].previous_state  = digitalRead(scan_pin[INDEX_SCPN_PIN_SET].pin );


    scan_pin[INDEX_SCAN_PIN_OK].pin             = PIN_OK;
    scan_pin[INDEX_SCAN_PIN_OK].pressed_state   = LOW;
    scan_pin[INDEX_SCAN_PIN_OK].debounce_time   = 30; // Debounce time in 30mS
    pinMode(scan_pin[INDEX_SCAN_PIN_OK].pin , INPUT_PULLUP);
    // When the system is starting capure the state 
    scan_pin[INDEX_SCAN_PIN_OK].previous_state  = digitalRead(scan_pin[INDEX_SCAN_PIN_OK].pin );
       
  
    /* 
     * OutPut pin Inits
     */
    output_pin[INDEX_OUT_PIN_RELAY].pin     = PIN_RELAY;
    pinMode(output_pin[INDEX_OUT_PIN_RELAY].pin , OUTPUT);
    digitalWrite(output_pin[INDEX_OUT_PIN_RELAY].pin , LOW);
        
}


/**********************************************************
 * 
 * function:  Module init like I2C LCD module
 * parent state call: main setup
 * 
***********************************************************/
void func_module_int(struct application_info* app)
{
  
#if (DEBUG_EN == 1) 
    Serial.begin(9600);
#endif
  
    lcd.init();            // Initialize the LCD
    lcd.backlight();       // On the LCD Back Light

    /* Init message to the LCD display */
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome");
    lcd.setCursor(0,1);
    lcd.print("Prabhat Roy");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome");
    lcd.setCursor(0,1);
    lcd.print("Dipali Roy");
    delay(2000);
    
#if (DEBUG_EN == 1)    
    Serial.println("[ OK ] Module init success!");
#endif
}




/**********************************************************
 * 
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
 * 
***********************************************************/

void func_ui_update(struct application_info* app)
{
    lcd.clear(); // clear all the contents

    switch(app->curr_state)
    {
        case STATE_IDLE:
            /* Timer part update */
            lcd.setCursor(0,0);
            lcd.print("Pump Time:");
            
            lcd.setCursor(11,0);
            lcd.print(pump_operation_time_min);
            lcd.setCursor(14,0);
            lcd.print("m");

            lcd.setCursor(0,1);
            lcd.print("Press START!");
        break;

        
        case STATE_RUNNING:
            lcd.setCursor(0,0);
            lcd.print("Set");
            lcd.setCursor(3,0);
            lcd.print(pump_operation_time_min);
            lcd.setCursor(6,0);
            lcd.print("m");

            /* Update the timer */
            lcd.setCursor(8,0);
            lcd.print("Run:");
            lcd.setCursor(12,0);
            
            if( (pump_current_time_update/60.0) < 1.0 )
            {
                lcd.print(pump_current_time_update);
                lcd.setCursor(15,0);
                lcd.print("s");
            }
            else
            {
                lcd.print(pump_current_time_update/60);
                lcd.setCursor(15,0);
                lcd.print("m");
            }

            lcd.setCursor(0,1);
            lcd.print("Pump Running...");
            
        break;


        case STATE_TIMER_SETUP:
            lcd.setCursor(0,0);
            lcd.print("Set Time:");

            lcd.setCursor(11,0);
            lcd.print(pump_operation_time_min);
            lcd.setCursor(14,0);
            lcd.print("m");

            lcd.setCursor(0,1);
            lcd.print("  UP/DOWN/SET");
            
        break;  
    }
}



/**********************************************************
 * 
 * state name:  input signal polling
 * parent state call: main loop
 * 
***********************************************************/
void io_signal_polling(struct application_info* app)
{
    for(int i = 0 ; i < MAX_IO_SCAN_PINS ; i++)
    {
        // Scan current pin state
        scan_pin[i].pin_state = digitalRead(scan_pin[i].pin); 
        
        if (scan_pin[i].pin_state != scan_pin[i].previous_state) 
        {
          scan_pin[i].last_debounce_time = millis();
        }

        /* After fluctuations debounce time settled */
        if ((millis() - scan_pin[i].last_debounce_time) >= scan_pin[i].debounce_time)
        {   
            // > because the micros are not precise eough
            if (scan_pin[i].pin_state != scan_pin[i].present_state)
            {
                scan_pin[i].present_state = scan_pin[i].pin_state;
                
                if( (scan_pin[i].present_state == scan_pin[i].pressed_state) && (scan_pin[i].pressed_state != BOTH_STATE) )
                { 
                  // A button press registered but not for both edge detection logic
                    scan_pin[i].is_pressed = CONDITION_REGISTERED;
#if (DEBUG_EN == 1)
                    Serial.print("[ IO Read ] button single edge: ");
                    Serial.println(scan_pin[i].pin);
#endif                    
                }
        
                if(scan_pin[i].pressed_state == BOTH_STATE)
                {
                    scan_pin[i].is_pressed = CONDITION_REGISTERED;
#if (DEBUG_EN == 1)
                    Serial.print("[ IO Read ] button both edge: ");
                    Serial.println(scan_pin[i].pin);
#endif
                }
            }
        }
        
        scan_pin[i].previous_state = scan_pin[i].pin_state;
    }

    // based on the physical signal  state machine sw signlas will be generated
    cb_state_machine_signal_gen(app);
}

/**********************************************************
 * 
 * state name:  state machine signal generation 
 *              based on the generate signal the state 
 *              change will happen 
 *              
 * parent state call: cb_signal_polling()
 * 
***********************************************************/

void cb_state_machine_signal_gen(struct application_info* app)
{
    for(int i = 0 ; i < MAX_IO_SCAN_PINS ; i++)
    {
        if(scan_pin[i].is_pressed == CONDITION_REGISTERED)
        {
            scan_pin[i].is_pressed = CONDITION_SERVED; // one time signal condition served

            switch(i)
            {   
                // dispense according to signal index
                //-------------------------------------------------
                case INDEX_SCAN_PIN_START:
                    app->cap_signal = SIGNAL_START; // timer up signal captured
                break;

                //-------------------------------------------------
                case INDEX_SCAN_PIN_UP:
                app->cap_signal = SIGNAL_UP; // timer up signal captured     
                break;

                //-------------------------------------------------
                case INDEX_SCAN_PIN_DOWN:
                  app->cap_signal = SIGNAL_DOWN; // Mode select signal captured
                break;

                //-------------------------------------------------
                case INDEX_SCAN_PIN_OK:
                  app->cap_signal = SIGNAL_OK; // Start signal captured
                break;

                //-------------------------------------------------
                case INDEX_SCPN_PIN_SET:
                  app->cap_signal = SIGNAL_SET; // Stop signal captured
                break;
           }
        }
    }
}


/**********************************************************
 * 
 * Pump ON signal generate 
 * 
***********************************************************/

void func_pump_start(void)
{
    digitalWrite(output_pin[INDEX_OUT_PIN_RELAY].pin , PUMP_ON_STATE);
}


/**********************************************************
 * 
 * Pump OFF signal generate 
 * 
***********************************************************/

void func_pump_stop(void)
{
    digitalWrite(output_pin[INDEX_OUT_PIN_RELAY].pin , PUMP_OFF_STATE);
}
