/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

/* MCAL includes. */
#include "uart0.h"
#include "MCAL/ADC/adc.h"
#include "MCAL/Temperatrue Sensor/lm35.h"
#include "gpio.h"
#include "tm4c123gh6pm_registers.h"
#include "GPTM.h"
/* Other includes */
#include <stdlib.h>

/* Delay function*/
#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 369
void Delay_MS(unsigned long long n)
{
    volatile unsigned long long count = 0;
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) );
}

/* Definitions for the event bits in the event group. */
#define mainSW2_INTERRUPT_BIT ( 1UL << 0UL )  /* Event bit 0, which is set by a SW2 Interrupt. */
#define mainSW1_INTERRUPT_BIT ( 1UL << 1UL )  /* Event bit 1, which is set by a SW1 Interrupt. */

#define Button_Control_Task_BIT ( 1UL << 0UL )          /* Event bit 0, which is set by ButtonControlTask for heat Control task. */
#define Temperature_Sensing_Task_BIT ( 1UL << 1UL )     /* Event bit 1, which is set by Temperature Sensing Task for heat control task. */


#define Button_Measure_Task_BIT ( 1UL << 0UL )
#define Temperature_Sensing_Measure_Task_BIT ( 1UL << 1UL )
#define LED_Measure_Task_BIT ( 1UL << 2UL )
#define Heat_Measure_Task_BIT ( 1UL << 3UL )
#define Display_Measure_Task_BIT ( 1UL << 4UL )
#define Diganostics_Measure_Task_BIT ( 1UL << 5UL )

#define Desired_Change_Display ( 1UL << 0UL )
#define Current_Change_Display ( 1UL << 1UL )

/*Task handles*/
TaskHandle_t xvButtonControlTask;
TaskHandle_t xvTemperatureSensingTask;
TaskHandle_t xvHeatingControlTask;
TaskHandle_t xvLedControlTask;
TaskHandle_t xvDisplay;
TaskHandle_t xvDiagnosticsTask;
TaskHandle_t xvRunTimeMeasurementsTask;

/*Event Groups*/
EventGroupHandle_t xEventGroupForButtons;              /*Two bits are set from ISR which unblocks Button task*/
EventGroupHandle_t xEventGroupForHeatingControlTask;   /*two bits are set, first from Button control and the other from Temp sensing*/
EventGroupHandle_t xEventGroupForDisplayTask;          /*two bits are set from Button control and sensing temp to unblock display task indicating for change */
EventGroupHandle_t xEventGroupForMeasurementTask;      /*bits are set from button task,sensing,heater, display,Led, Diagnostic to unblock Measurment task*/


/*Mutex and sempahores*/
xSemaphoreHandle xMutexPassengerControl;       /*Used by button control for passenger*/
xSemaphoreHandle xMutexDriverControl;          /*Used by button control for passenger*/

xSemaphoreHandle xMutexPassengerSensingTemperature; /*Used by Sensing Task for passenger*/
xSemaphoreHandle xMutexDriverSensingTemperature;    /*Used by Sensing Task for Driver*/

xSemaphoreHandle xMutexDriverControlSeatTemperature;    /*Used by Control Task for Driver*/
xSemaphoreHandle xMutexPassengerControlSeatTemperature; /*Used by Control Task for Passenger*/

xSemaphoreHandle xLeds;         /*Used by led task to unblock */

xSemaphoreHandle xDiagnostic;   /*Used by heater control task to unblock */
xSemaphoreHandle xTempToDig;    /*Used by Diagnostic task to unblock */

/* shared resources*/
typedef enum {Off,Low=25,Medium=30,High=35}Desierd_Heating_Levels;  /*Range of each level*/
typedef enum {Offheat,Lowheat,Mediumheat,Highheat}Heater_Levels;    /*the heater output level based on desired*/

uint32_t Passenger_Seat_Button_Interrupt_Flag=Off; /*Global Variable for the Passenger seat Button to be used by the interrupt*/
uint32_t Driver_Seat_Button_Interrupt_Flag=Off;    /*Global Variable for the Driver seat Button to be used by the interrupt*/

uint32_t Passenger_Current_Temperature=0;          /*Global Variable for the Passenger LM35(Temp sensor) Value*/
uint32_t Driver_Current_Temperature=0;             /*Global Variable for the Driver LM35(Temp sensor) Value*/

uint32_t Driver_Heater=Offheat;                    /*Global Variable for the Driver heater Value (default OFF)*/
uint32_t Passenger_Heater=Offheat;                 /*Global Variable for the Passenger heater Value (default OFF)*/

/*Global variable to calc and check execution time for each task*/
uint32_t ButtonControlTaskExecutiontime=0;
uint32_t SensingTempTaskExecutiontime=0;
uint32_t DisplayTaskExecutiontime=0;
uint32_t HeatingControlTaskExecutiontime=0;
uint32_t LedControlTaskExecutiontime=0;
uint32_t DiganosticsControlTaskExecutiontime=0;
uint32_t ButtonTotalTime=0;
uint32_t TempsenseTotalTime=0;
uint32_t HeatControlTotalTime=0;
uint32_t DisplayTotalTime=0;
uint32_t LedControlTotalTime=0;
uint32_t DiagnosticsTotalTime=0;

/* The HW setup function */
static void prvSetupHardware( void );

/* FreeRTOS tasks */
void vButtonControlTask(void *pvParameters);         /*Unblock by Port F Handler*/
void vTemperatureSensingTask(void *pvParameters);    /*Used to Measure the LM-35 Temp is global variables*/
void vHeatingControlTask(void *pvParameters);        /*Used to determine Heater level based on Temp. Sensing Task*/
void vLedControlTask(void *pvParameters);            /*Control Led OutPut*/
void vDisplayTask (void *pvParameters);              /*Display UART */
void vDiagnosticsTask (void *pvParameters);          /*Task used to assure range of heater from 5 to 40*/
void vRunTimeMeasurementsTask(void *pvParameters);   /*Measure runtimr for each task and cpu load*/

/* Define the strings that will be passed in as the task parameters. */


int main()
 {
    /* Setup the hardware for use with the Tiva C board. */
    prvSetupHardware();

    /*Creating Mutex and Sempahores*/
    xMutexPassengerControl = xSemaphoreCreateMutex();
    xMutexDriverControl = xSemaphoreCreateMutex();

    xMutexPassengerSensingTemperature = xSemaphoreCreateMutex();
    xMutexDriverSensingTemperature = xSemaphoreCreateMutex();

    xMutexDriverControlSeatTemperature = xSemaphoreCreateMutex();
    xMutexPassengerControlSeatTemperature = xSemaphoreCreateMutex();


    xLeds = xSemaphoreCreateBinary();

    xDiagnostic = xSemaphoreCreateBinary();

    xTempToDig= xSemaphoreCreateBinary();

    /* Creating Event Groups */
    xEventGroupForButtons = xEventGroupCreate();            /*Event for Button Task*/
    xEventGroupForHeatingControlTask= xEventGroupCreate();  /*Event for Heater Task*/
    xEventGroupForDisplayTask = xEventGroupCreate();        /*Event for Display Task*/
    xEventGroupForMeasurementTask = xEventGroupCreate();    /*Event for Measurement Task*/


    /* Create Tasks here */
    /*
     * Functionality:  Monitors button inputs to cycle through the heater states(Off,Low,Medium,High)
     * Implementation: 1- Using GPIO to read Interrupts on the Button Clicks
     *                 2- When button is pressed, the heating level should advance from Off-->Low-->Medium-->High-->Low
     *                 3- Set the heating level using global shared variables
     *                 (Passenger_Seat_Button_Interrupt_Flag,Driver_Seat_Button_Interrupt_Flag) which is protected using
     *                 (xMutexPassengerControl,xMutexDriverControl)
     * Interaction with Other Tasks:
     *                 1-The Heating Control Task will read the updated heating level from the shared variable
     *                  (Passenger_Seat_Button_Interrupt_Flag,Driver_Seat_Button_Interrupt_Flag)
     *                 2-The Display Update Task will display the updated level on the shared screen
     */
    xTaskCreate(vButtonControlTask,         /* Pointer to the function that implements the task. */
                "Task for button control",  /* Text name for the task.  This is to facilitate debugging only. */
                256,                        /* Stack depth - most small microcontrollers will use much less stack than this. */
                NULL,                       /* We are not passing a task parameter in this example. */
                3,                          /* This task will run at priority 4. */
                &xvButtonControlTask);                      /* We are not using the task handle. */

    /*Temperature sensing task
     * Functionality: 1-Reads temperature from the sensor (or potentiometer for testing purposes) and maps the analog value to a valid temperature range (5°C to 40°C).
                      2-This task continuously monitors temperature changes.
     * Implementation: 1-Use *ADC* to read the analog voltage from the sensor or potentiometer.
                       2-Convert the ADC value to temperature using a formula.
                       3-Store the current temperature in xMutexPassengerSensingTemperature and xMutexDriverSensingTemperature.
     *Interaction with Other Tasks:
     *                 1-The Heating Control Task reads the current temperature from this task.
     *                 2-The Diagnostic Task will check if the temperature is within the valid range and act accordingly.
     * */
    xTaskCreate(vTemperatureSensingTask,             /* Pointer to the function that implements the task. */
                "Task for Temperature sensing",      /* Text name for the task.  This is to facilitate debugging only. */
                256,                                 /* Stack depth - most small microcontrollers will use much less stack than this. */
                NULL,                                /* We are not passing a task parameter in this example. */
                3,                                   /* This task will run at priority 3. */
                &xvTemperatureSensingTask);
    /*Heating control Task*/
    xTaskCreate(vHeatingControlTask,                 /* Pointer to the function that implements the task. */
                "Task for Heating Control",              /* Text name for the task.  This is to facilitate debugging only. */
                256,                                     /* Stack depth - most small microcontrollers will use much less stack than this. */
                NULL,                                    /* We are not passing a task parameter in this example. */
                4,                                       /* This task will run at priority 2. */
                &xvHeatingControlTask);
    xTaskCreate(vLedControlTask,                 /* Pointer to the function that implements the task. */
                "Task for LED Control",              /* Text name for the task.  This is to facilitate debugging only. */
                256,                                     /* Stack depth - most small microcontrollers will use much less stack than this. */
                NULL,                                    /* We are not passing a task parameter in this example. */
                2,                                       /* This task will run at priority 2. */
                &xvLedControlTask);

    xTaskCreate(vDisplayTask,                 /* Pointer to the function that implements the task. */
                   "DisplayTask",              /* Text name for the task.  This is to facilitate debugging only. */
                   256,                                     /* Stack depth - most small microcontrollers will use much less stack than this. */
                   NULL,                                    /* We are not passing a task parameter in this example. */
                   1,                                       /* This task will run at priority 2. */
                   &xvDisplay);
    xTaskCreate(vDiagnosticsTask,                 /* Pointer to the function that implements the task. */
                "DiagnosticsTask",              /* Text name for the task.  This is to facilitate debugging only. */
                256,                                     /* Stack depth - most small microcontrollers will use much less stack than this. */
                NULL,                                    /* We are not passing a task parameter in this example. */
                5,                                       /* This task will run at priority 2. */
                &xvDiagnosticsTask);
    xTaskCreate(vRunTimeMeasurementsTask,                 /* Pointer to the function that implements the task. */
                    "xvRunTimeMeasurementsTask",              /* Text name for the task.  This is to facilitate debugging only. */
                    256,                                     /* Stack depth - most small microcontrollers will use much less stack than this. */
                    NULL,                                    /* We are not passing a task parameter in this example. */
                    6,                                       /* This task will run at priority 2. */
                    &xvRunTimeMeasurementsTask);

   // UART0_SendString("Main \r\n");

    /* Now all the tasks have been started - start the scheduler.

    NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
    The processor MUST be in supervisor mode when vTaskStartScheduler is
    called.  The demo applications included in the FreeRTOS.org download switch
    to supervisor mode prior to main being called.  If you are not using one of
    these demo application projects then ensure Supervisor mode is used here. */
    vTaskStartScheduler();

    /* Should never reach here!  If you do then there was not enough heap
    available for the idle task to be created. */
    for (;;);

}


static void prvSetupHardware( void )
{
    /* Place here any needed HW initialization such as GPIO, UART, etc.  */
    UART0_Init();
    ADC_Init();
    GPIO_BuiltinButtonsLedsInit();
    GPIO_SW1EdgeTriggeredInterruptInit();
    GPIO_SW2EdgeTriggeredInterruptInit();
    GPTM_WTimer0Init();
}
void vButtonControlTask(void *pvParameters)
{

    uint32 GPTM1,GPTM2;
    GPTM1=GPTM_WTimer0Read();
    EventBits_t xEventGroupValue;

    const EventBits_t xBitsToWaitFor = ( mainSW1_INTERRUPT_BIT | mainSW2_INTERRUPT_BIT);

   // UART0_SendString("entered Buttom control task \r\n");
    for (;;)
    {
        /* Block to wait for event bits to become set within the event group. */
        xEventGroupValue = xEventGroupWaitBits( xEventGroupForButtons,    /* The event group to read. */
                                                xBitsToWaitFor, /* Bits to test. */
                                                pdTRUE,         /* Clear bits on exit if the unblock condition is met. */
                                                pdFALSE,        /* Don't wait for all bits. */
                                                portMAX_DELAY); /* Don't time out. */
       // UART0_SendString(" Buttom control task - after event group \r\n");

        /* In case PF0 edge triggered interrupt occurred, it will set event 0 bit */
        if ((xEventGroupValue & mainSW2_INTERRUPT_BIT) != 0)
        {
            /* Acquire the Passenger mutex */
            if (xSemaphoreTake(xMutexPassengerControl, portMAX_DELAY) == pdTRUE)
            {
                //UART0_SendString(" Buttom control task - xMutexPassengerControl true  \r\n");
               // Delay_MS(60);
               /// vTaskDelay(pdMS_TO_TICKS(100));

            /* Access the Passenger Shared Button resource */
                if(Passenger_Seat_Button_Interrupt_Flag==Off)
                    {
                    Passenger_Seat_Button_Interrupt_Flag=Low;
                    //UART0_SendString(" Buttom control task - passenger heater low  \r\n");
                    //Delay_MS(60);
                    //vTaskDelay(pdMS_TO_TICKS(100));
                    }
                else if(Passenger_Seat_Button_Interrupt_Flag==Low)
                    {
                    Passenger_Seat_Button_Interrupt_Flag=Medium;
                    //UART0_SendString(" Buttom control task - passenger heater Medium  \r\n");
                  //  Delay_MS(60);
                   // vTaskDelay(pdMS_TO_TICKS(100));

                    }
                else if(Passenger_Seat_Button_Interrupt_Flag==Medium)
                    {
                    Passenger_Seat_Button_Interrupt_Flag=High;
                   // UART0_SendString(" Buttom control task - passenger heater high  \r\n");
                    //Delay_MS(60);
                    //vTaskDelay(pdMS_TO_TICKS(100));

                    }
                else if(Passenger_Seat_Button_Interrupt_Flag==High)
                    {
                    Passenger_Seat_Button_Interrupt_Flag=Off;
                   // UART0_SendString(" Buttom control task - passenger heater off  \r\n");
                    //Delay_MS(60);
                    //vTaskDelay(pdMS_TO_TICKS(100));
                    }
            /* Release the Passenger mutex */
            xSemaphoreGive(xMutexPassengerControl);
            }
        }
        /* In case PF4 edge triggered interrupt occurred, it will set event 1 bit */
        if ((xEventGroupValue & mainSW1_INTERRUPT_BIT) != 0)
        {
           if (xSemaphoreTake(xMutexDriverControl, portMAX_DELAY) == pdTRUE)
           {
              // UART0_SendString(" Buttom control task - xMutexDriverControl true  \r\n");
                   //vTaskDelay(pdMS_TO_TICKS(100));
            /* Access the Driver Shared Button resource */
                if(Driver_Seat_Button_Interrupt_Flag==Off)
                   {
                    Driver_Seat_Button_Interrupt_Flag=Low;
                   // UART0_SendString(" Buttom control task - driver heater low  \r\n");
                    //Delay_MS(60);
                    //vTaskDelay(pdMS_TO_TICKS(100));

                   }
                else if(Driver_Seat_Button_Interrupt_Flag==Low)
                   {
                    Driver_Seat_Button_Interrupt_Flag=Medium;
                   // UART0_SendString(" Buttom control task - driver medium   \r\n");
                   // Delay_MS(60);
                   // vTaskDelay(pdMS_TO_TICKS(100));

                   }
                else if(Driver_Seat_Button_Interrupt_Flag==Medium)
                   {
                    Driver_Seat_Button_Interrupt_Flag=High;
                   // UART0_SendString(" Buttom control task - driver high  \r\n");
                    // Delay_MS(60);
                    //vTaskDelay(pdMS_TO_TICKS(100));

                   }
                else if(Driver_Seat_Button_Interrupt_Flag==High)
                   {
                    Driver_Seat_Button_Interrupt_Flag=Off;
                   // UART0_SendString(" Buttom control task - driver off  \r\n");
                     //Delay_MS(60);
                    //vTaskDelay(pdMS_TO_TICKS(100));

                   }
             /* Release the Driver mutex */
              xSemaphoreGive(xMutexDriverControl);
            }
        }
        xEventGroupSetBits(xEventGroupForHeatingControlTask, Button_Control_Task_BIT);/*Setting the bit for Heating control task to start working*/
        xEventGroupSetBits(xEventGroupForDisplayTask ,Desired_Change_Display );/*Setting the bit for Heating control task to start working*/
        xEventGroupSetBits(xEventGroupForMeasurementTask,Button_Measure_Task_BIT );
       GPTM2=GPTM_WTimer0Read();
       ButtonControlTaskExecutiontime=(GPTM2-GPTM1)*10;
       ButtonTotalTime=GPTM_WTimer0Read()*10;
       vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void vTemperatureSensingTask(void *pvParameters)
{
    uint32 GPTM1,GPTM2;
    GPTM1=GPTM_WTimer0Read();
    TickType_t xLastWakeTime = xTaskGetTickCount(); /*Getting the current time to start counting from their*/
        for (;;)
        {
          //  UART0_SendString("vTemperatureSensingTask\r\n:");

            /*Getting both mutex to make sure we update the temperature values at the same time  */
            if ((xSemaphoreTake(xMutexPassengerSensingTemperature, portMAX_DELAY) == pdTRUE) )
              {
                            //UART0_SendString("passenger temp is:");/*PE3 - PASSENGER*/
                       Passenger_Current_Temperature=LM35_getTemperature(SENSOR0_CHANNEL_ID);/*calculating temperature on passenger sensor*/
                            //UART0_SendInteger(Passenger_Current_Temperature);
                           // UART0_SendString("\r\n");
              }

            if ( (xSemaphoreTake(xMutexDriverSensingTemperature, portMAX_DELAY)) == pdTRUE  )
            {
                    Driver_Current_Temperature=LM35_getTemperature(SENSOR1_CHANNEL_ID);   /*calculating temperature on driver sensor*/
                              // UART0_SendString(" Driver temp is:"); /*PE2 - DRIVER*/

                               //UART0_SendInteger(Driver_Current_Temperature);
                               //UART0_SendString("\r\n");
            }


             /*release The Driver and Passenger Temp Mutexes*/
            xSemaphoreGive(xMutexPassengerSensingTemperature);
            xSemaphoreGive(xMutexDriverSensingTemperature);
            xEventGroupSetBits(xEventGroupForHeatingControlTask, Temperature_Sensing_Task_BIT);/*Setting the bit for Heating control task to start working*/

            xEventGroupSetBits(xEventGroupForDisplayTask ,Current_Change_Display );/*Setting the bit for D task to start working*/

            xSemaphoreGive(xTempToDig);
            xEventGroupSetBits(xEventGroupForMeasurementTask, Temperature_Sensing_Measure_Task_BIT );



            GPTM2=GPTM_WTimer0Read();
            SensingTempTaskExecutiontime=(GPTM2-GPTM1)*10;
            TempsenseTotalTime=GPTM_WTimer0Read();

            /*blocking the function for 1 second*/
            vTaskDelay(pdMS_TO_TICKS(500));


        }
}



void vHeatingControlTask(void *pvParameters)
{
    uint32 GPTM1,GPTM2;
    GPTM1=GPTM_WTimer0Read();
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = (Button_Control_Task_BIT | Temperature_Sensing_Task_BIT );
    char DriverTemperatureDifference=0;
    char PassengerTemperatureDifference=0;
    for (;;)
    {
    if((xSemaphoreTake(xDiagnostic, portMAX_DELAY) == pdTRUE) )
    {
            //UART0_SendString("HeatingControlTask before \r\n");

            /* Block to wait for event bits to become set within the event group. */
            xEventGroupValue = xEventGroupWaitBits( xEventGroupForHeatingControlTask,    /* The event group to read. */
                                                    xBitsToWaitFor,                      /* Bits to test. */
                                                    pdTRUE,                              /* Clear bits on exit if the unblock condition is met. */
                                                    pdFALSE,                             /* Don't Wait for all bits. */
                                                    portMAX_DELAY);                      /* Don't time out. */

     if ((xSemaphoreTake(xMutexDriverControlSeatTemperature, portMAX_DELAY) == pdTRUE) && (xSemaphoreTake(xMutexDriverControl, portMAX_DELAY) == pdTRUE)   )
     {
         //DriverTemperatureDifference=Driver_Seat_Button_Interrupt_Flag - Driver_Current_Temperature ;

       //  UART0_SendString("semaphore Driver TRUE\r\n");
       if( Driver_Seat_Button_Interrupt_Flag==Off)
           {
                  // UART0_SendString("DDRIVER SEAT OFF\r\n");
                   Driver_Heater=Offheat;

           }
       else if ( Driver_Seat_Button_Interrupt_Flag != Off)
           {
               DriverTemperatureDifference=Driver_Seat_Button_Interrupt_Flag - Driver_Current_Temperature ;
               if(Driver_Seat_Button_Interrupt_Flag >Driver_Current_Temperature)
               {
                  // UART0_SendString("DriverTemperatureDifference>0 \r\n");
                   if(DriverTemperatureDifference>=10)
                       {
                           Driver_Heater=Highheat;
                          // UART0_SendString("Driver is high \r\n");

                       }

                   else if((DriverTemperatureDifference>=5) && (DriverTemperatureDifference<10))
                       {
                           Driver_Heater=Mediumheat;
                          // UART0_SendString("Driver is medium \r\n");

                       }

                   else if((DriverTemperatureDifference>=2) && (DriverTemperatureDifference<5))
                       {
                          // UART0_SendString("Driver is low \r\n");
                           Driver_Heater=Lowheat;

                       }

                   else
                       {
                           Driver_Heater=Offheat;
                           //UART0_SendString(" DriverTemperatureDifference =OFF \r\n");

                       }
               }

               else
               {
                   Driver_Heater=Offheat;
               }

           }

     }

     if ( (xSemaphoreTake(xMutexPassengerControlSeatTemperature, portMAX_DELAY) == pdTRUE)&& (xSemaphoreTake(xMutexPassengerControl, portMAX_DELAY) == pdTRUE)  )
       {
        // PassengerTemperatureDifference=Passenger_Seat_Button_Interrupt_Flag - Passenger_Current_Temperature ;

             //UART0_SendString("semaphore Passenger TRUE\r\n");
         if (Passenger_Seat_Button_Interrupt_Flag==Off )
           {

                           //  UART0_SendString(" Driver_Heater=Offheat \r\n");
                             Passenger_Heater=Offheat;
                            //vTaskDelay(pdMS_TO_TICKS(100));
           }

         else if (Passenger_Seat_Button_Interrupt_Flag !=Off )
         {
             PassengerTemperatureDifference=Passenger_Seat_Button_Interrupt_Flag - Passenger_Current_Temperature ;


               if(Passenger_Seat_Button_Interrupt_Flag>Passenger_Current_Temperature)
              {
                   if(PassengerTemperatureDifference>=10)
                        {
                           Passenger_Heater=Highheat;
                           //UART0_SendString(" Passenger is HIGH\r\n");

                        }
                   else if((PassengerTemperatureDifference>=5) && (PassengerTemperatureDifference<10))
                        {
                           Passenger_Heater=Mediumheat;
                           //UART0_SendString(" Passenger is MEDIUM\r\n");

                        }
                   else if((PassengerTemperatureDifference>=2) && (PassengerTemperatureDifference<5))
                        {
                           Passenger_Heater=Lowheat;
                           //UART0_SendString(" Passenger is LOW\r\n");

                        }
                   else
                        {
                           Passenger_Heater=Offheat;
                          //UART0_SendString(" Passenger is LOW\r\n");

                        }
                }
                else
                {
                    Passenger_Heater=Offheat;
                    //UART0_SendString(" Passenger is LOW\r\n");
                }
            }


        }

     xSemaphoreGive(xMutexDriverControlSeatTemperature);
     xSemaphoreGive(xMutexPassengerControlSeatTemperature);

     xSemaphoreGive(xMutexDriverControl);
     xSemaphoreGive(xMutexPassengerControl);

     xSemaphoreGive(xLeds);
     xEventGroupSetBits(xEventGroupForMeasurementTask, Heat_Measure_Task_BIT );
     GPTM2=GPTM_WTimer0Read();
     HeatingControlTaskExecutiontime=(GPTM2-GPTM1)*10;
     HeatControlTotalTime=GPTM_WTimer0Read()*10;


    // UART0_SendString(" END OF HeatingControlTask  \r\n");

     //vTaskDelay(pdMS_TO_TICKS(1000));

    }
    } /*for*/
 } /*function */



void vLedControlTask(void *pvParameters)
{
    uint32 GPTM1,GPTM2;
    GPTM1=GPTM_WTimer0Read();
    for(;;)
    {


    if (xSemaphoreTake(xLeds, portMAX_DELAY) == pdTRUE)
        {


            if(Driver_Heater==Offheat)
            {
                GPIO_BlueLedOff();
                GPIO_GreenLedOff();
               //ART0_SendString(" ****************DRIVER__OFF****************  \r\n");

               // GPIO_RedLedToggle(); ////test
            }
            else if(Driver_Heater==Lowheat)
            {
                GPIO_GreenLedOn();
               //ART0_SendString(" ****************DRIVER_LOW****************  \r\n");


            }
            else if(Driver_Heater==Mediumheat)
            {
                GPIO_GreenLedOff();
               GPIO_BlueLedOn();
               // UART0_SendString(" ****************DRIVER_MEDIUM****************  \r\n");

            }
            else if (Driver_Heater==Highheat)
            {
                GPIO_GreenLedOn();
                GPIO_BlueLedOn();
               // UART0_SendString(" ****************DRIVER_HIGH****************  \r\n");

            }
            if(Passenger_Heater==Offheat)
            {
                GPIO_BlueLedOff();
                GPIO_GreenLedOff();
               // UART0_SendString(" ****************PASSENGER_OFF****************  \r\n");

            }
            else if(Passenger_Heater==Lowheat)
            {
                GPIO_GreenLedOn();
               // UART0_SendString(" ****************PASSENGER_LOW****************  \r\n");

            }
            else if(Passenger_Heater==Mediumheat)
            {
                GPIO_GreenLedOff();
                GPIO_BlueLedOn();
               // UART0_SendString(" ****************PASSENGER_MEDIUM****************  \r\n");

            }
            else if (Passenger_Heater==Highheat)
            {
                GPIO_GreenLedOn();
                GPIO_BlueLedOn();
               // UART0_SendString(" ****************PASSENGER_HIGH****************  \r\n");

            }
        }





    xEventGroupSetBits(xEventGroupForMeasurementTask, LED_Measure_Task_BIT );
    GPTM2=GPTM_WTimer0Read();
    LedControlTaskExecutiontime=(GPTM2-GPTM1)*10;
    LedControlTotalTime=GPTM_WTimer0Read()*10;
    ///vTaskDelay(pdMS_TO_TICKS(2000));
    }
}


void vDisplayTask (void *pvParameters)
{
    uint32 GPTM1,GPTM2;
    GPTM1=GPTM_WTimer0Read();
        EventBits_t xEventGroupValue;
        const EventBits_t xBitsToWaitFor = ( Current_Change_Display| Desired_Change_Display);



        for (;;)
        {

    /* Block to wait for event bits to become set within the event group. */
          xEventGroupValue = xEventGroupWaitBits( xEventGroupForDisplayTask,    /* The event group to read. */
                                                  xBitsToWaitFor,                      /* Bits to test. */
                                                   pdTRUE,                              /* Clear bits on exit if the unblock condition is met. */
                                                   pdFALSE,                             /* Don't Wait for all bits. */
                                                   portMAX_DELAY);                      /* Don't time out. */




          if (  (xSemaphoreTake(xMutexPassengerControl, portMAX_DELAY) == pdTRUE)  && (xSemaphoreTake(xMutexPassengerSensingTemperature, portMAX_DELAY) == pdTRUE) && (xSemaphoreTake(xMutexPassengerControlSeatTemperature, portMAX_DELAY) == pdTRUE)  )
          {

              UART0_SendString("Passenger Desired Temp =");
              UART0_SendInteger(Passenger_Seat_Button_Interrupt_Flag);
              UART0_SendString("\r\n");

              UART0_SendString("Passenger Current Temp =");
              UART0_SendInteger(Passenger_Current_Temperature);
               UART0_SendString("\r\n");


               if ( Passenger_Heater == Offheat)
               {
                   UART0_SendString("Passenger Heater Level OFF HEAT\r\n");

               }
               else if (Passenger_Heater ==Lowheat )
               {
                   UART0_SendString("Passenger Heater Level LOW HEAT\r\n");

               }

               else if (Passenger_Heater ==Mediumheat )
               {
                   UART0_SendString("Passenger Heater Level MEDIUM HEAT\r\n");

                }
               else if (Passenger_Heater == Highheat )
               {
                 UART0_SendString("Passenger Heater Level HIGH HEAT\r\n");

               }


          }

          if (  (xSemaphoreTake(xMutexDriverControl, portMAX_DELAY) == pdTRUE)  && (xSemaphoreTake(xMutexDriverSensingTemperature, portMAX_DELAY) == pdTRUE) && (xSemaphoreTake(xMutexDriverControlSeatTemperature, portMAX_DELAY) == pdTRUE)  )
           {

              UART0_SendString("Driver Desired Temp =");
              UART0_SendInteger(Driver_Seat_Button_Interrupt_Flag);
              UART0_SendString("\r\n");

                UART0_SendString("Driver Current Temp =");
                UART0_SendInteger(Driver_Current_Temperature);
                UART0_SendString("\r\n");


              if ( Driver_Heater == Offheat)
             {
                 UART0_SendString("Driver Heater Level OFF HEAT\r\n");

              }
              else if (Driver_Heater ==Lowheat )
                {
                    UART0_SendString("Driver Heater Level LOW HEAT");

               }

              else if (Driver_Heater ==Mediumheat )
               {
                  UART0_SendString("Driver Heater Level MEDIUM HEAT");

                  }
                   else if (Driver_Heater == Highheat )
                    {
                   UART0_SendString("Driver Heater Level HIGH HEAT");

                  }

                            Delay_MS(3000);

           }

          xSemaphoreGive(xMutexDriverControlSeatTemperature);
          xSemaphoreGive(xMutexPassengerControlSeatTemperature);

          xSemaphoreGive(xMutexDriverControl);
          xSemaphoreGive(xMutexPassengerControl);

          xSemaphoreGive(xMutexPassengerSensingTemperature);
          xSemaphoreGive(xMutexDriverSensingTemperature);
          xEventGroupSetBits(xEventGroupForMeasurementTask, Display_Measure_Task_BIT );
          GPTM2=GPTM_WTimer0Read();
          DisplayTaskExecutiontime=(GPTM2-GPTM1)*10;
          DisplayTotalTime=GPTM_WTimer0Read()*10;



         // vTaskDelay(pdMS_TO_TICKS(1000));
        }
}
void vDiagnosticsTask (void *pvParameters)
{
    uint32 GPTM1,GPTM2;
    GPTM1=GPTM_WTimer0Read();
    for(;;)
    {
        if (xSemaphoreTake(xTempToDig, portMAX_DELAY) == pdTRUE)
        {

             if ((xSemaphoreTake(xMutexPassengerSensingTemperature, portMAX_DELAY) == pdTRUE) && (xSemaphoreTake(xMutexDriverSensingTemperature, portMAX_DELAY) == pdTRUE))
             {
               if(Passenger_Current_Temperature<5 || Passenger_Current_Temperature>40 || Driver_Current_Temperature<5 || Driver_Current_Temperature>40 )
               {
                   uint32_t GPTM=GPTM_WTimer0Read()*10;

                    Driver_Heater=Offheat;                    /*Global Variable for the Driver heater Value*/
                    Passenger_Heater=Offheat;

                    GPIO_BlueLedOff();
                    GPIO_GreenLedOff();

                   GPIO_RedLedOn();
                   UART0_SendString("Error Time stamp at:");
                   UART0_SendInteger(GPTM);
                   UART0_SendString("\r\n");

                   UART0_SendString("Driver Current Temp:");
                   UART0_SendInteger(Driver_Current_Temperature);
                   UART0_SendString("\r\n");

                   UART0_SendString("Passenger Current Temp:");
                   UART0_SendInteger(Passenger_Current_Temperature);
                   UART0_SendString("\r\n");

                  // xSemaphoreGive(xDiagnostic);


               }
               else
               {
                   GPIO_RedLedOff();
                   xSemaphoreGive(xDiagnostic);
               }



           }
             xSemaphoreGive(xMutexPassengerSensingTemperature);
             xSemaphoreGive(xMutexDriverSensingTemperature);
      }
        xEventGroupSetBits(xEventGroupForMeasurementTask, Diganostics_Measure_Task_BIT );
        GPTM2=GPTM_WTimer0Read();
        DiganosticsControlTaskExecutiontime=(GPTM2-GPTM1)*10;
        DiagnosticsTotalTime=GPTM_WTimer0Read()*10;
    }
}
void vRunTimeMeasurementsTask(void *pvParameters)
{
    EventBits_t xEventGroupValue;
    uint32 ullCPU_Load;
    const EventBits_t xBitsToWaitFor = (Button_Measure_Task_BIT|Temperature_Sensing_Measure_Task_BIT|
                                        LED_Measure_Task_BIT|Heat_Measure_Task_BIT| Display_Measure_Task_BIT|
                                        Diganostics_Measure_Task_BIT);
    for(;;)
    {
        xEventGroupValue = xEventGroupWaitBits( xEventGroupForMeasurementTask,    /* The event group to read. */
                                                         xBitsToWaitFor,                      /* Bits to test. */
                                                          pdTRUE,                              /* Clear bits on exit if the unblock condition is met. */
                                                          pdTRUE,                             /* Wait for all bits. */
                                                          portMAX_DELAY);                      /* Don't time out. */

        UART0_SendString("ButtonTaskExecutionTime=");
        UART0_SendInteger(ButtonControlTaskExecutiontime);
        UART0_SendString("\r\n");
        UART0_SendString("SensingTemperatureTaskExecutionTime=");
        UART0_SendInteger(SensingTempTaskExecutiontime);
        UART0_SendString("\r\n");
        UART0_SendString("DisplayTaskExecutionTime=:");
        UART0_SendInteger(DisplayTaskExecutiontime);
        UART0_SendString("\r\n");
        UART0_SendString("HeatingControlTaskExecutionTime=");
        UART0_SendInteger(HeatingControlTaskExecutiontime);
        UART0_SendString("\r\n");
        UART0_SendString("LedControlTaskExecutionTime=");
        UART0_SendInteger(LedControlTaskExecutiontime);
        UART0_SendString("\r\n");
        UART0_SendString("DiaganosticsTaskExecutionTime=");
        UART0_SendInteger(DiganosticsControlTaskExecutiontime);
        UART0_SendString("\r\n");
        ullCPU_Load=(ButtonTotalTime+TempsenseTotalTime+HeatControlTotalTime+DisplayTotalTime+LedControlTotalTime+DiagnosticsTotalTime)*100/GPTM_WTimer0Read();
        UART0_SendString("CPULoad=");
        UART0_SendInteger(ullCPU_Load);
        UART0_SendString("\r\n");

    }
}


/* GPIO PORTF External Interrupt - ISR */
void GPIOPortF_Handler(void)
{
    UART0_SendString("ISR \r\n");

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(GPIO_PORTF_RIS_REG & (1<<0))           /* PF0 handler code */
    {
        Delay_MS(200);
        xEventGroupSetBitsFromISR(xEventGroupForButtons, mainSW2_INTERRUPT_BIT,&xHigherPriorityTaskWoken);
        GPIO_PORTF_ICR_REG   |= (1<<0);       /* Clear Trigger flag for PF0 (Interrupt Flag) */

    }
    else if(GPIO_PORTF_RIS_REG & (1<<4))      /* PF4 handler code */
    {
        Delay_MS(200);
        xEventGroupSetBitsFromISR(xEventGroupForButtons, mainSW1_INTERRUPT_BIT,&xHigherPriorityTaskWoken);
        GPIO_PORTF_ICR_REG   |= (1<<4);       /* Clear Trigger flag for PF4 (Interrupt Flag) */

    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/
