#define SINGLE_MODE 1
#define TWO_PLAYERS_MODE 2
#define EMPTY 0
#define X_ 1
#define O_ 2
#define NO_WINNER 0
#define BOARD_SIZE 9

int indicatorLedBlue = 22;
int indicatorLedRed = 23;
boolean turn = true;
int gameMode = SINGLE_MODE;
int row = 0;
int board[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
int oldBoard[] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
int redLeds [] = {11, 12, 13, 14, 15, 16, 17, 18, 19};
int blueLeds [] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
int buttonPins[] = {53, 52, 51, 50, 49, 48, 47, 46, 45};

void setup(){
  Serial.begin(9600);
  for(int i = 0; i < BOARD_SIZE; i++){
    pinMode(buttonPins[i], INPUT);
    digitalWrite(buttonPins[i], HIGH);
    pinMode(redLeds[i], OUTPUT);
    pinMode(blueLeds[i], OUTPUT);
    digitalWrite(redLeds[i], LOW);
    digitalWrite(blueLeds[i], LOW);
  }
  pinMode(indicatorLedBlue, OUTPUT);
  pinMode(indicatorLedRed, OUTPUT);
  changePlayer();
}

int DCgap = 150;            // max ms between clicks for a double click event
int holdTime = 2000;        // ms hold period: how long to wait for press+hold event

// Button variables
boolean buttonVals[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};   // value read from button
boolean buttonLasts[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};  // buffered value of the button's previous state
boolean DCwaitings[] = {false, false, false, false, false, false, false, false, false};  // whether we're waiting for a double click (down)
boolean DConUps[] = {false, false, false, false, false, false, false, false, false};     // whether to register a double click on next release, or whether to wait and click
boolean singleOKs[] = {true, true, true, true, true, true, true, true, true};    // whether it's OK to do a single click
long downTimes[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};         // time the button was pressed down
long upTimes[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1}; // time the button was released
boolean ignoreUps[] = {false, false, false, false, false, false, false, false, false};   // whether to ignore the button release because the click+hold was triggered
boolean waitForUps[] = {false, false, false, false, false, false, false, false, false};        // when held, whether to wait for the up event
boolean holdEventPasts[] = {false, false, false, false, false, false, false, false, false};    // whether or not the hold event happened already


int checkButton(int button) { 
   int event = 0;
   buttonVals[button] = digitalRead(buttonPins[button]);
   // Button pressed down
   if (buttonVals[button] == HIGH && buttonLasts[button] == LOW)
   {
       downTimes[button] = millis();
       ignoreUps[button] = false;
       waitForUps[button] = false;
       singleOKs[button] = true;
       holdEventPasts[button] = false;
       if ((millis()-upTimes[button]) < DCgap && DConUps[button] == false && DCwaitings[button] == true)  DConUps[button] = true;
       else  DConUps[button] = false;
       DCwaitings[button] = false;
   }
   // Button released
   else if (buttonVals[button] == LOW && buttonLasts[button] == HIGH)
   {        
       if (not ignoreUps[button])
       {
           upTimes[button] = millis();
           if (DConUps[button] == false) DCwaitings[button] = true;
           else
           {
               event = 2;
               DConUps[button] = false;
               DCwaitings[button] = false;
               singleOKs[button] = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVals[button] == LOW && (millis()-upTimes[button]) >= DCgap && DCwaitings[button] == true && DConUps[button] == false && singleOKs[button] == true && event != 2)
   {
       event = 1;
       DCwaitings[button] = false;
   }
   // Test for hold
   if (buttonVals[button] == HIGH && (millis() - downTimes[button]) >= holdTime) {
       // Trigger long press
       if (not holdEventPasts[button])
       {
           event = 3;
           waitForUps[button] = true;
           ignoreUps[button] = true;
           DConUps[button] = false;
           DCwaitings[button] = false;
           //downTime = millis();
           holdEventPasts[button] = true;
       }
   }
   buttonLasts[button] = buttonVals[button];
   return event*10 + button;
}

int readButtons(){
  while(1){
    for (int i = 0; i < BOARD_SIZE; i++){
      int a = checkButton(i);
      if (a > 9)
        return a;
    }
  }
  return -1;
}
void changePlayer(){
  if(turn){
    digitalWrite(indicatorLedBlue, LOW);
    digitalWrite(indicatorLedRed, HIGH);
  } else{
    digitalWrite(indicatorLedRed, LOW);
    digitalWrite(indicatorLedBlue, HIGH);
  }
  turn = !turn;
  row++;
}
  

void play(int value){
  if(board[value] != EMPTY){
    return;
  }
  for(int i = 0; i < BOARD_SIZE; i++){
    oldBoard[i] = board[i];
  }
  if(turn){
    digitalWrite(redLeds[value],HIGH);
    board[value] = X_;
  } else{
    digitalWrite(blueLeds[value],HIGH);
    board[value] = O_;
  }
  changePlayer();
}

int checkForWinner(){
  if((board[0] == board[1]) && (board[1] == board[2])){
    return board[0];
  }else if((board[3] == board[4]) && (board[4] == board[5])){
    return board[3];
  }else if((board[6] == board[7]) && (board[7] == board[8])){
    return board[6];
  }else if((board[0] == board[3]) && (board[3] == board[6])){
    return board[0];
  }else if((board[1] == board[4]) && (board[4] == board[7])){
    return board[1];
  }else if((board[2] == board[5]) && (board[5] == board[8])){
    return board[2];
  }else if((board[0] == board[4]) && (board[4] == board[8])){
    return board[0];
  }else if((board[2] == board[4]) && (board[4] == board[6])){
    return board[2];
  }
  return 0;
}

void winning(int winner){
  if(turn){
    digitalWrite(indicatorLedBlue, LOW);
  } else{
    digitalWrite(indicatorLedRed, LOW);
  }
  if(winner != 1){
    for(int i = 0; i < BOARD_SIZE; i++){
      digitalWrite(redLeds[i], LOW);
    }
    for(int i = 0; i < 5; i++){
      for(int j = 0; j < BOARD_SIZE; j++){
        digitalWrite(blueLeds[j], HIGH);
      }
      delay(500);
      for(int j = 0; j < BOARD_SIZE; j++){
        digitalWrite(blueLeds[j], LOW);
      }
      delay(500);
    }
  }else{
    for(int i = 0; i < BOARD_SIZE; i++){
      digitalWrite(blueLeds[i], LOW);
    }
    for(int i = 0; i < 5; i++){
      for(int j = 0; j < BOARD_SIZE; j++){
        digitalWrite(redLeds[j], HIGH);
      }
      delay(500);
      for(int j = 0; j < BOARD_SIZE; j++){
        digitalWrite(redLeds[j], LOW);
      }
      delay(500);
    }
  }
  
}

void drow_blink(){
  if(turn){
    digitalWrite(indicatorLedBlue, LOW);
  } else{
    digitalWrite(indicatorLedRed, LOW);
  }
  for(int i = 0; i<BOARD_SIZE; i++){
  if(board[i] == X_){
    digitalWrite(redLeds[i], LOW);
  }else if(board[i] == O_)
    digitalWrite(blueLeds[i], LOW);
  }
  for(int j = 0; j < 5; j++){
    for(int i = 0; i<BOARD_SIZE; i++){
      if(board[i] == 1){
        digitalWrite(redLeds[i], HIGH);
      }else if(board[i] == 2)
        digitalWrite(blueLeds[i], HIGH);
    }
    delay(500);
    for(int i = 0; i<BOARD_SIZE; i++){
      if(board[i] == 1){
        digitalWrite(redLeds[i], LOW);
      }else if(board[i] == 2)
        digitalWrite(blueLeds[i], LOW);
    }
    delay(500);
    
  }
}

void undo(){
  for(int i = 0; i < BOARD_SIZE; i++){
    board[i] = oldBoard[i];
  }
  if(turn){
    digitalWrite(indicatorLedBlue, LOW);
    digitalWrite(indicatorLedRed, HIGH);
  } else{
    digitalWrite(indicatorLedRed, LOW);
    digitalWrite(indicatorLedBlue, HIGH);
  }
  turn = !turn;
  row--;
  for(int i = 0; i<BOARD_SIZE; i++){
    if(board[i] == 0){
      digitalWrite(redLeds[i], LOW);
      digitalWrite(blueLeds[i], LOW);
    }
  }
  Serial.println("UNDO");
}

void restartGame(){
  turn = true;
  row = 0;
  for(int i = 0; i < BOARD_SIZE; i++){
    digitalWrite(redLeds[i], LOW);
    digitalWrite(blueLeds[i], LOW);
    board[i] = 0;
    oldBoard[i]=0;
  }
  Serial.println("RESTARTED");
}

void decodeButtonVlues(int value){
  int event = value/10;
  int button = value%10;
  if(event == 1){
    play(button);
  }else if(event == 2 && gameMode == TWO_PLAYERS_MODE){
    undo();
  }else if(event == 3){
    restartGame();
  }
}

int computeTurn(){
  int positions[8];
  int positions_count = 0; 
  for(int i = 0; i < BOARD_SIZE; i++){
    if(board[i] == EMPTY){
      positions[positions_count++] = i;
    }
  }
  for(int i = 0; i < positions_count; i++){
    int current_position = positions[i];
    board[current_position] = O_;
    if(checkForWinner() == O_){
      board[current_position] = EMPTY;
      return current_position;
    }
    board[current_position] = EMPTY;
  }
  return positions[0];
}

void loop(){
  if(gameMode == TWO_PLAYERS_MODE || (gameMode == SINGLE_MODE && turn)){
    decodeButtonVlues(readButtons());
  }else{
    int arduinoTurn = computeTurn();
    Serial.print("Aduino turn: ");
    Serial.println(arduinoTurn);
    play(arduinoTurn);
  }
  int winner = checkForWinner();
  if(row == 10 && winner == NO_WINNER){
    drow_blink();
    restartGame();
  }
  if(winner != NO_WINNER){
    winning(winner);
    restartGame();
  }
}
