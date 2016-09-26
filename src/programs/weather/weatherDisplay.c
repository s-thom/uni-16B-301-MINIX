#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <minix/subserve.h>
#include <time.h>

/* define the struct */
typedef struct weather WEATHER;
struct weather{
  char discription[8];
  int windspeed;
  char wind_direction[2];
  int temp;
  int humidity;
  struct tm timeOfPush;
};

void display(WEATHER p){
  
  printf("---------------------------------------------\n");
  printf("Weather at time : %s\n", asctime(&p.timeOfPush));
  printf("it is : %s\n", p.discription);
  printf("wind direction : %s\n", p.wind_direction);
  printf("wind speed : %d Km/h\n", p.windspeed);
  printf("temperature : %d°c\n", p.temp);
  printf("humidity : %d%%\n", p.humidity);
  printf("---------------------------------------------\n");

}

int main(int argc, char **argv){
  int waitTime = 1;
  int op;
  WEATHER w;  

  if(argc == 2) waitTime = atoi(argv[1]);

  /* subscribe to server */
  op = subscribe("localWeather");
  if(op == 0){
   printf("weather channel could not be found :(\n");
   return;
  }
  
  /* update loop */
  while(1){
    while(!(pull("localWeather", &w, sizeof(WEATHER)))){
      /* loop waiting 4 update */
      sleep(waitTime);
    }
    display(w);
  }
  
}
