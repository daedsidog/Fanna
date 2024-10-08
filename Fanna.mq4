//——————————CONSTANTS—————————//

const int HINDSIGHT = 48;
const int COOLDOWN = 10000;
const long LEARNING_INTERVAL = 24 * 60 * 60;
const double LOT_SHARE = 0.1;
const double OFFSET = 0.002;

//———————GLOBAL VARIABLES—————//

static datetime g_time_last_learned = 0;

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

#define PI double offset, int hindsight, int length, char &pair[], char &interval[], const double &opening_price[], const double &closing_price[], const double &max_price[], const double &min_price[], double &volume[]
#define PIARGS OFFSET, HINDSIGHT, length, pair, interval, opening_price, closing_price, max_price, min_price, volume

#import "Fanna.dll"
int reset_fanna(PI);
int train_fanna(PI);
int build_fanna_database(PI);
double pulse_fanna(PI);
#import

void OnTick(){
   if(FANNA_INSTRUCTION == TRADE && OrdersTotal() == 0){
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
      if(TimeCurrent() - g_time_last_learned >= LEARNING_INTERVAL){
         build_fanna_database(PIARGS);
         train_fanna(PIARGS);
         g_time_last_learned = TimeCurrent();
      }
      double available_lots =   (AccountEquity() * AccountLeverage()) / MarketInfo(Symbol(), MODE_LOTSIZE);
      double sentiment = pulse_fanna(PIARGS);
      PrintFormat("Fanna sentiment: %lf", sentiment);
      if(sentiment > 0.75){
         // —————————————————————————————Buy———————————————————————————
            double
            SL = Ask - OFFSET,
            TP = Ask + OFFSET;
         int buy_order = OrderSend(Symbol(), OP_BUY, available_lots * LOT_SHARE ,Ask , 3, SL, TP, "Buy" ,16384 ,0 ,clrGreen);
      }
      else if(sentiment < 0.25){
         // —————————————————————————————Sell——————————————————————————
         double
            SL = Bid + OFFSET,
            TP = Bid - OFFSET;
         int sell_order = OrderSend(Symbol(), OP_SELL, available_lots * LOT_SHARE ,Bid , 3, SL, TP, "Sell" ,16384 ,0 ,clrRed);
      }
   }
}
int OnInit(){
   if(FANNA_INSTRUCTION == NONE){
      ExpertRemove();
      return(INIT_SUCCEEDED);
   }
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
      build_fanna_database(PIARGS);
      ExpertRemove();
      break;
   case RESET:
      reset_fanna(PIARGS);
      ExpertRemove();
      break;
   case TRAIN:
      train_fanna(PIARGS);
      ExpertRemove();
      break;
   }
   return(INIT_SUCCEEDED);
}