typedef struct
 {
     byte one;
     byte two;
     byte three;
 }  record_type;

record_type record[8];

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:

  record[0] = (record_type) {1,2,3};
  record[1] = (record_type) {4,5,6};
  record[2] = (record_type) {7,8,9};
  record[3] = (record_type) {10,11,12};
  record[4] = (record_type) {13,14,15};
  record[5] = (record_type) {16,17,18};
  record[6] = (record_type) {19,20,21};
  record[7] = (record_type) {22,23,24};

  for(int x=0; x<8; x++) {  
    Serial.print(record[x].one);
    Serial.print("-");
    Serial.print(record[x].two);
    Serial.print("-");
    Serial.println(record[x].three);
  }

//  record[0] = 0;

//  for(int x=0; x<8; x++) {  
//    Serial.println(record[x].one);
//  }
  
//  byte history[2][3] = {
//    {1,2,3},
//    {4,5,6},
//    {7,8,9}
//    };
//  for(byte x=0; x<=1; x++) {  
//    for(byte y=0; y<=2; y++) {  
//      Serial.println(history[x][y]);
//    }
//  }
//  for(byte x=0; x<=1; x++) {  
//    for(byte y=0; y<=2; y++) {  
//      if (x < 1){
//        history[x][y] = history[x + 1][y];  
//      }
//      
//    }
//  }
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {    // is a character available?
    char in = Serial.read();       // get the character
    if (in == 's') {
      push();
      show();
      Serial.println();
    } else if(in == 'p'){
      pop();
      show();
      Serial.println();
    } else if(in == 'f'){
      if (search()){
        Serial.println("ada nih");  
      } else {
        Serial.println("gaada kok");  
      }
      show();
      Serial.println();
    }
  }
}

void pop() {
  for(int x=0; x<8; x++) {  
    if(x < 7){
      if(record[x].one == 0 && record[x].two == 0){
        return;
      }
      record[x].one = record[x+1].one;
      record[x].two = record[x+1].two;
      record[x].three = record[x+1].three;
    } else {
      record[x].one = 0;
      record[x].two = 0;
      record[x].three = 0;
    }
  }
}

void push() {
  for(int x=0; x<8; x++) {  
    if(x < 7){
      if(record[x].one == 0 && record[x].two == 0){
        record[x] = (record_type) {5,5,5};
        return;
      }
    } else {
      if(record[x].one == 0 && record[x].two == 0){
        record[x] = (record_type) {5,5,5};
      } else {
        pop();
        record[x] = (record_type) {5,5,5};
      }
    }
  }
}

void show() {
  for(int x=0; x<8; x++) {  
    Serial.print(record[x].one);
    Serial.print("-");
    Serial.print(record[x].two);
    Serial.print("-");
    Serial.println(record[x].three);
  }
}

bool search() {
  for(int x=0; x<8; x++) {  
    if(record[x].one == 5 && record[x].two == 5 && record[x].three == 5){
      return 1;
    }
  }
  return 0;
}

