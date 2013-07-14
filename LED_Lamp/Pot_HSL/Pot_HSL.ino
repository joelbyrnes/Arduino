
int Rpin=10;
int Gpin=9;
int Bpin=11;
float H,S,L,Rval,Gval,Bval;

void HSL(float H, float S, float L, float& Rval, float& Gval, float& Bval);
float Hue_2_RGB( float v1, float v2, float vH );

void setup()
{
  Serial.begin(9600);

  // set up pot

  pinMode(12, OUTPUT);
}


void loop()
{
  //H=rotary.position();
  H = 1;
  S=1;
  L=.5;
  Rval=0;
  Gval=0;
  Bval=0;
  HSL(float(H)/360,S,L,Rval,Gval,Bval);

  //common anode configuration
  //analogWrite(Rpin, 255-Rval);
  //analogWrite(Gpin, 255-Gval);
  //analogWrite(Bpin, 255-Bval);
  //digitalWrite(12,HIGH);

  //common cathode configuration
  analogWrite(Rpin, Rval);
  analogWrite(Gpin, Gval);
  analogWrite(Bpin, Bval);
  digitalWrite(12,LOW);

  //print statements for debug
  Serial.print("position:");
  Serial.print(H);
  Serial.print(" R:");
  Serial.print(Rval);
  Serial.print(" G:");
  Serial.print(Gval);
  Serial.print(" B:");
  Serial.println(Bval);
  delay(1000);
}

void HSL(float H, float S, float L, float& Rval, float& Gval, float& Bval)
{
  float var_1;
  float var_2;
  float Hu=H+.33;
  float Hd=H-.33;
  if ( S == 0 )                       //HSL from 0 to 1
  {
     Rval = L * 255;                      //RGB results from 0 to 255
     Gval = L * 255;
     Bval = L * 255;
  }
  else
  {
     if ( L < 0.5 ) 
       var_2 = L * ( 1 + S );
     else           
       var_2 = ( L + S ) - ( S * L );
 
     var_1 = 2 * L - var_2;
 
     Rval = round(255 * Hue_2_RGB( var_1, var_2, Hu ));
     Serial.print("Rval:");
     Serial.println(Hue_2_RGB( var_1, var_2, Hu ));
     Gval = round(255 * Hue_2_RGB( var_1, var_2, H ));
     Bval = round(255 * Hue_2_RGB( var_1, var_2, Hd ));
  }
}

float Hue_2_RGB( float v1, float v2, float vH )             //Function Hue_2_RGB
{
   if ( vH < 0 )
     vH += 1;
   if ( vH > 1 )
     vH -= 1;
   if ( ( 6 * vH ) < 1 )
     return ( v1 + ( v2 - v1 ) * 6 * vH );
   if ( ( 2 * vH ) < 1 )
     return ( v2 );
   if ( ( 3 * vH ) < 2 )
     return ( v1 + ( v2 - v1 ) * (.66-vH) * 6 );
   return ( v1 );
}

void HslToRgb( int hue, byte sat, byte light, byte* lpOutRgb ) {
    if( sat == 0 ) {
        lpOutRgb[0] = lpOutRgb[1] = lpOutRgb[2] = light;
        return;
    }
    float nhue = (float)hue * (1.0f / 360.0f);
    float nsat = (float)sat * (1.0f / 255.0f);
    float nlight = (float)light * (1.0f / 255.0f);
    float m2;
    if (light < 128 ) m2 = nlight * ( 1.0f + nsat );
    else m2 = ( nlight + nsat ) - ( nsat * nlight );
    float m1 = ( 2.0f * nlight ) - m2;
    lpOutRgb[0] = HueToChannel( m1, m2, nhue + (1.0f / 3.0f) );
    lpOutRgb[1] = HueToChannel( m1, m2, nhue );
    lpOutRgb[2] = HueToChannel( m1, m2, nhue - (1.0f / 3.0f) );
}

byte HueToChannel( float m1, float m2, float h ) {
    if( h < 0.0f ) h += 1.0f;
    if( h > 1.0f ) h -= 1.0f;
    if( ( 6.0f * h ) < 1.0f ) return (byte)( 255.0f * ( m1 + ( m2 - m1 ) * 6.0f * h ) );
    if( ( 2.0f * h ) < 1.0f ) return (byte)( 255.0f * m2 );
    if( ( 3.0f * h ) < 2.0f ) return (byte)( 255.0f * ( m1 + ( m2 - m1 ) * ((2.0f/3.0f) - h) * 6.0f ) );
    return (byte)( 255.0f * m1 );
}

