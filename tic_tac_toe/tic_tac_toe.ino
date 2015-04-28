int buttons[] = {45, 46, 47, 48, 49, 50, 51, 52, 53};
int indicatorLed1 = 40;
int indicatorLed2 = 41;
boolean turn = true;
int board[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int redLeds [] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
int blueLeds [] = {11, 12, 13, 14, 15, 16, 17, 18, 19};
void setup(){
  Serial.begin(9600);
  for(int i = 0; i < 9; i++){
    pinMode(buttons[i], INPUT);
    pinMode(redLeds[i], OUTPUT);
    pinMode(blueLeds[i], OUTPUT);
    digitalWrite(redLeds[i], LOW);
    digitalWrite(blueLeds[i], LOW);
  }
  pinMode(indicatorLed1, OUTPUT);
  pinMode(indicatorLed2, OUTPUT);
  changePlayer();
}
int readButtons(){
  while(1){
    if(digitalRead(buttons[0]) == HIGH){
      return 0;
    } else if (digitalRead(buttons[1]) == HIGH){
      return 1;
    } else if (digitalRead(buttons[2]) == HIGH){
      return 2;
    } else if (digitalRead(buttons[3]) == HIGH){
      return 3;
    } else if (digitalRead(buttons[4]) == HIGH){
      return 4;
    } else if (digitalRead(buttons[5]) == HIGH){
      return 5;
    } else if (digitalRead(buttons[6]) == HIGH){
      return 6;
    } else if (digitalRead(buttons[7]) == HIGH){
      return 7;
    } else if (digitalRead(buttons[8]) == HIGH){
      return 8;
    }
  }
  return 0;
}

void changePlayer(){
  if(turn){
    digitalWrite(indicatorLed1, LOW);
    digitalWrite(indicatorLed2, HIGH);
  } else{
    digitalWrite(indicatorLed2, LOW);
    digitalWrite(indicatorLed1, HIGH);
  }
  turn = !turn;
}
  

void play(int value){
  if(board[value]!=0){
    return;
  }
  if(turn){
    digitalWrite(redLeds[value],HIGH);
    board[value] = 1;
  } else{
    digitalWrite(blueLeds[value],HIGH);
    board[value] = 2;
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
  if(winner == 1){
    for(int i = 0; i < 9; i++){
      digitalWrite(redLeds[i], LOW);
    }
    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 9; j++){
        digitalWrite(blueLeds[j], HIGH);
      }
      delay(500);
      for(int j = 0; j < 9; j++){
        digitalWrite(blueLeds[j], LOW);
      }
      delay(500);
    }
  }else{
    for(int i = 0; i < 9; i++){
      digitalWrite(blueLeds[i], LOW);
    }
    for(int i = 0; i < 3; i++){
      for(int j = 0; j < 9; j++){
        digitalWrite(redLeds[j], HIGH);
      }
      delay(500);
      for(int j = 0; j < 9; j++){
        digitalWrite(redLeds[j], LOW);
      }
      delay(500);
    }
  }
}

void loop(){
  play(readButtons());
  int winner = checkForWinner();
  if(winner != 0){
    winning(winner);
  }
};
