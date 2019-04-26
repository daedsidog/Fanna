//——————————CONSTANTS—————————//

const int SAMPLES = 1024;
const int HINDSIGHT = 64;
const int FORESIGHT = 32;
const int COOLDOWN = 10000;
const double LOT_SHARE = 0.1;

//———————GLOBAL VARIABLES—————//

static bool g_sleeping = false;

//————————————————————————————//


#property show_inputs
#property strict

enum fanna_instruction_enum {
   NONE,
   BUILD_DATABASE,
   RESET,
   TRAIN,
   TRADE
};

input fanna_instruction_enum FANNA_INSTRUCTION = NONE;

#define PI int hindsight, int foresight, int length, char &pair[], char &interval[], const double &opening_price[], const double &closing_price[], const double &max_price[], const double &min_price[], double &volume[]
#define PIARGS HINDSIGHT, FORESIGHT, length, pair, interval, opening_price, closing_price, max_price, min_price, volume

#import "Fanna.dll"
int reset_fanna(PI);
int train_fanna(PI);
int build_fanna_database(PI, int samples);
double pulse_fanna(PI);
#import

int OnInit(){
   if(FANNA_INSTRUCTION == NONE)
      ExpertRemove();
   int length = Bars;
   double opening_price[], closing_price[], max_price[], min_price[], volume[];
   ArrayResize(opening_price, Bars);
   ArrayResize(closing_price, Bars);
   ArrayResize(max_price, Bars);
   ArrayResize(min_price, Bars);
   ArrayResize(volume, Bars);
   char pair[], interval[];
   StringToCharArray(Symbol(), pair);
   StringToCharArray(IntegerToString(Period()), interval);
   for(int i = 0; i < Bars; ++i){
      opening_price[i] = double(Open[i]);
      closing_price[i] = double(Close[i]);
      max_price[i] = double(High[i]);
      min_price[i] = double(Low[i]);
      volume[i] = double(Volume[i]);
   }
   switch(FANNA_INSTRUCTION){
   case BUILD_DATABASE:
      build_fanna_database(PIARGS, SAMPLES);
      break;
   case RESET:
      reset_fanna(PIARGS);
      break;
   case TRAIN:
      train_fanna(PIARGS);
      break;
   }
   ExpertRemove();
   return(INIT_SUCCEEDED);
}

void OnDeinit(const int reason){
}

void OnTick(){
      if(FANNA_INSTRUCTION == TRADE && OrdersTotal() == 0 && !g_sleeping){
      double avg = 0.0;
      for(int i = 0; i < HINDSIGHT; ++i){
         avg += High[i] - Low[i];
      }
      avg /= double(HINDSIGHT);
      int length = Bars;
      double opening_price[], closing_price[], max_price[], min_price[], volume[];
      ArrayResize(opening_price, HINDSIGHT);
      ArrayResize(closing_price, HINDSIGHT);
      ArrayResize(max_price, HINDSIGHT);
      ArrayResize(min_price, HINDSIGHT);
      ArrayResize(volume, HINDSIGHT);
      char pair[], interval[];
      StringToCharArray(Symbol(), pair);
      StringToCharArray(IntegerToString(Period()), interval);
      for(int i = 0; i < HINDSIGHT; ++i){
         opening_price[i] = double(Open[i]);
         closing_price[i] = double(Close[i]);
         max_price[i] = double(High[i]);
         min_price[i] = double(Low[i]);
         volume[i] = double(Volume[i]);
      }
      double sentiment = pulse_fanna(PIARGS);
      PrintFormat("Fanna sentiment: %lf", sentiment);
      double available_lots =   (AccountEquity() * AccountLeverage()) / MarketInfo(Symbol(), MODE_LOTSIZE);
      if(sentiment >= 0.75){
         // —————————————————————————————Buy———————————————————————————
         int buy_order = OrderSend(Symbol(), OP_BUY, available_lots * LOT_SHARE ,Ask , 3, Close[0] - avg, Close[0] + avg, "Buy" ,16384 ,0 ,clrGreen);
      }
      else if(sentiment <= 0.25){
         // —————————————————————————————Sell——————————————————————————
         int sell_order = OrderSend(Symbol(), OP_SELL, available_lots * LOT_SHARE ,Bid , 3, Close[0] + avg, Close[0] - avg, "Sell" ,16384 ,0 ,clrRed);
      }
      else {
         g_sleeping = true;
         Sleep(COOLDOWN);
         g_sleeping = false;
      }
   }
}

