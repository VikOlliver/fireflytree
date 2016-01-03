/* fireflytree - lights up a matrix of LED "fireflies"

 (C)2016 vik@diamondage.co.nz GPLv3 applies
 */

#define MAX_ROWS  3
#define MAX_COLS  5

#define NORMAL_FIREFLIES  4
#define MAX_FIREFLIES     min(12,MAX_ROWS*MAX_COLS)  // Maximum 12 firelfies or full matrix
#define FIRE_DIM_MAX  512  // Maximum value of dimming (8 pulses)
#define DIM_SHIFT    4
#define DIM_MAX      (FIRE_DIM_MAX>>DIM_SHIFT)  // Calc number of pulses
#define MS_DELAY      50  // ms per dimming pulse

#define TAP_PIN  6
#define BLINK_PIN  13

int ledMatrix[MAX_COLS][MAX_ROWS];
int rowpins[]={2,3,4};
int colpins[]={9,10,11,12};
int fireflyCount=0;
boolean  wasTapped=false;

// Blank all the objects
void blankMatrix() {
  int i,j;
  for (i=0; i<MAX_ROWS; i++)
    for (j=0; j<MAX_COLS; j++)
      ledMatrix[j][i]=0;
  fireflyCount=0;
}

// Display all the objects
void displayMatrix() {
  int i,j,dim;
  for (i=0; i<MAX_ROWS; i++)
    for (j=0; j<MAX_COLS; j++) {
      if (ledMatrix[j][i]>0) {
        // The firefly is alive. Light it up in proportion to the amount of life it has left.
        digitalWrite(colpins[j],HIGH);
        digitalWrite(rowpins[i],LOW);
        dim=(ledMatrix[j][i]--)>>DIM_SHIFT;
        delayMicroseconds(MS_DELAY*dim+1);
        digitalWrite(colpins[j],LOW);
        digitalWrite(rowpins[i],HIGH);
        delayMicroseconds(MS_DELAY*(DIM_MAX-dim));
        // If the firefly gets too dim to see, zombie respawn or kill it.
        if (dim<1) {
          if (random(4)>1) {
            // I live! Again!
            ledMatrix[j][i]=FIRE_DIM_MAX/2;
          } else {
            // Dead firefly
            ledMatrix[j][i]=0;
            fireflyCount--;
          }
        }
        // If the sensor has been tapped, flag it.
        if ((digitalRead(TAP_PIN)==LOW)&&(digitalRead(TAP_PIN)==LOW)) wasTapped=true;
      }
    }    
}

void makeFirefly(int initBright) {
  int x,y;

  // Don't make too many fireflies.
  if (fireflyCount>=MAX_FIREFLIES) return;  

  // Loop until we hit a vacant firefly slot.
  while (true) {
    x=random(MAX_COLS);
    y=random(MAX_ROWS);
    if (ledMatrix[x][y]==0) {
      // If the user specified a brightness, use it. Otherwise select at random.
      ledMatrix[x][y]=((initBright==0)?random(FIRE_DIM_MAX/2,FIRE_DIM_MAX):initBright);
      break;
    }
  }
  fireflyCount++;
}

void setup() {
  int i;
  pinMode(TAP_PIN,INPUT);
  pinMode(BLINK_PIN,OUTPUT);
  // Set all row/col pins and blank the output
  for (i=0; i<MAX_ROWS; i++) {
    pinMode(rowpins[i],OUTPUT);
    digitalWrite(rowpins[i],HIGH);
  }
  for (i=0; i<MAX_COLS; i++) {
    pinMode(colpins[i],OUTPUT);
    digitalWrite(colpins[i],LOW);
  }
  blankMatrix();
  // Turn on all the fireflies we're allowed to, so the LEDs are tested.
  while (fireflyCount<MAX_FIREFLIES) makeFirefly(FIRE_DIM_MAX);
  wasTapped=false;
}

void loop() {
  int i,t;
  while (true) {
    // Flip round forever, illuminating fireflies.
    for (i=0; i<70; i++) {
      displayMatrix();
      // If someone taps the base, make lots of fireflies.
      if (wasTapped) {
        wasTapped=false;
        // Pin13 light indicates a tap trigger for debugging.
        digitalWrite(BLINK_PIN,HIGH);
        while (fireflyCount<MAX_FIREFLIES) makeFirefly(random(FIRE_DIM_MAX*2/3,FIRE_DIM_MAX));
      }
    }
    // Occasionally spawn a new one.
    if ((fireflyCount<NORMAL_FIREFLIES)&&(random(5)==0)) makeFirefly(0);
    // Very occasionally spawn a bunch of 'em
    if (random(90)==1) {
      t=MAX_FIREFLIES-sqrt(random(MAX_FIREFLIES*MAX_FIREFLIES));  // Low probability of lots of fireflies.
      while (fireflyCount<t) makeFirefly(0);
    }
    // Turn off the tap indicator.
    digitalWrite(BLINK_PIN,LOW);
  }
}  
