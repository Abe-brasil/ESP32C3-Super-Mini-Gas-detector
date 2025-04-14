float sensor_volt= 0;
float RS = 0;           //  Get the value of RS via in a clear air
float R0 = 0;           // Get the value of R0 via in Alcohol
float sensorValue = 0;
int Adc_Res = 4096;  //Analog Digital Converter depending bit resolution - 10 bit=1024, 12 bit=4096, 16 bit=65536 etc.
float Max_V_Adc = 5.0; //maxADC Converter Input Voltage/ with voltage devider to mcu stays at 5vdc

void setup(){
   pinMode(34, INPUT);
   Serial.begin(115200);   
}
 
void loop(){
  for(int i = 0 ; i < 100 ; i++){
    sensorValue = sensorValue + analogRead(34);
  }
  delay(10);
  sensorValue = sensorValue/100.0;        //get average of reading
  sensor_volt = sensorValue * Max_V_Adc /(Adc_Res);
  Serial.print("Volt: ");
  Serial.println(sensor_volt);
  
  RS = (Max_V_Adc - sensor_volt)/sensor_volt; 
  R0 = RS/50.0;                           //50 is found using interpolation from the graph
  
  Serial.print("R0 = ");
  Serial.println(R0,5);
  delay(100);
  sensorValue = 0; 
}