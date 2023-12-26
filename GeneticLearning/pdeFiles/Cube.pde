class ActualCube {
    
  //final float wallSize = 30;
  
  class Cubit {
    
    color[] colors;
    float radX = 0, radY = 0, radZ = 0;
    float wallSize;
    int xInit, yInit, zInit;
    
    Cubit(color[] colors, int xInit, int yInit, int zInit, float wallSize) {
      this.colors = colors; //front, right, back, left, top, bottom
      this.xInit = xInit;
      this.yInit = yInit;
      this.zInit = zInit;
      this.wallSize = wallSize;
    }
    
    void drawCubit(float x, float y, float z) {
      pushMatrix();
      translate(x,y,z);
      rotateX(radX);
      rotateY(radY);
      rotateZ(radZ);
      
      rectMode(CENTER);
      strokeWeight(2);
      for(int i = 0; i < colors.length; i++) {
        
        if(colors[i] == color(0))
          continue;
          
        pushMatrix();
        
        fill(colors[i]);
        switch(i) {
          case 0:
            translate(0, 0, -wallSize/2);
            break;
            
          case 1:
            translate(wallSize/2, 0, 0);
            rotateY(PI/2);
            break;
            
          case 2:
            translate(0, 0, wallSize/2);
            break;
            
          case 3:
            translate(-wallSize/2, 0, 0);
            rotateY(PI/2);
            break;
            
          case 4:
            translate(0, wallSize/2, 0);
            rotateX(PI/2);
            break;
            
          case 5:
            translate(0, -wallSize/2, 0);
            rotateX(PI/2);
            break;
        }
        rect(0,0,wallSize,wallSize);
        
        popMatrix();
      }
      
      popMatrix();
    }
    
    void addRotation(float radX, float radY, float radZ) {
      
      color temp;
      if(radZ == -PI/2) {
        temp = colors[1];
        colors[1] = colors[5];
        colors[5] = colors[3];
        colors[3] = colors[4];
        colors[4] = temp;
      } else if(radZ == PI/2) {
        temp = colors[4];
        colors[4] = colors[3];
        colors[3] = colors[5];
        colors[5] = colors[1];
        colors[1] = temp;
      } else if(radY == PI/2) {
        temp = colors[0];
        colors[0] = colors[1];
        colors[1] = colors[2];
        colors[2] = colors[3];
        colors[3] = temp;
      } else if(radY == -PI/2) {
        temp = colors[3];
        colors[3] = colors[2];
        colors[2] = colors[1];
        colors[1] = colors[0];
        colors[0] = temp;
      } else if(radX == -PI/2) {
        temp = colors[0];
        colors[0] = colors[5];
        colors[5] = colors[2];
        colors[2] = colors[4];
        colors[4] = temp;
      } else if(radX == PI/2) {
        temp = colors[4];
        colors[4] = colors[2];
        colors[2] = colors[5];
        colors[5] = colors[0];
        colors[0] = temp;
      }
      /*color[] cubitColors = {
            (i==x-1) ? colorArr[2]:color(0), //green
            (k==0) ? colorArr[1]:color(0),  //yellow
            (i==0) ? colorArr[0]:color(0),  //blue
            (k==z-1) ? colorArr[3]:color(0), //white
            (j==y-1) ? colorArr[5]:color(0), //orange
            (j==0) ? colorArr[4]:color(0), //red
          };*/
      
    }
    
  }
  
  Cubit[][][] cubits;
  final color[] colorArr = {color(86, 161, 214), color(237, 237, 57), color(88, 204, 65), color(255), color(224, 76, 49), color(232, 173, 72)};
  //blue, yellow, green, white, red, orange
  int x, y, z;
  float wallSize;
  
  //drawing is based on the x,y,z being in the middle of the ActualCube
  ActualCube(int x, int y, int z, float wallSize) /* x,y,z are for the # of cubits */ {
    this.x = x;
    this.y = y;
    this.z = z;
    this.wallSize = wallSize;
    
    cubits = new Cubit[x][y][z];
    
    for(int i = 0; i < x; i++) {
      for(int j = 0; j < y; j++) {
        for(int k = 0; k < z; k++) {
                        
          //blue-x=0, green-x=max, white-z=max, yellow-k=0, red-y=0, ora 
          color[] cubitColors = {
            (k==0) ? colorArr[0]:color(0), //blue
            (i==x-1) ? colorArr[4]:color(0),  //orange
            (k==z-1) ? colorArr[2]:color(0),  //green
            (i==0) ? colorArr[5]:color(0), //red
            (j==y-1) ? colorArr[1]:color(0), //yellow
            (j==0) ? colorArr[3]:color(0), //white
          };
          
          //if(i==0 && j ==0 && k==0)
          //{
          //  cubitColors[0] = colorArr[2];
          //  cubitColors[1] = colorArr[2];
          //  cubitColors[2] = colorArr[2];
          //  cubitColors[3] = colorArr[2];
          //  cubitColors[4] = colorArr[2];
          //  cubitColors[5] = colorArr[2];
          //}
          
          cubits[i][j][k] = new Cubit(cubitColors, i, j, k, this.wallSize);
          
        }
      }
    }
    
  }
  
  void drawActualCube(float posX, float posY, float posZ, float radX, float radY) {
        
    pushMatrix();
    translate(posX, posY, posZ);
    rotateX(-radX);
    rotateY(-radY);
    rotateZ(PI);
   
    for(int i = 0; i < x; i++) {
      for(int j = 0; j < y; j++) {
        for(int k = 0; k < z; k++) {
                    
          if(i != 0 && i != x-1 && j != 0 && j != y-1 && k != 0 && k != z-1)
            continue;

          int[] signs = { -(x-1)/2+i, -(y-1)/2+j, -(z-1)/2+k };
          cubits[i][j][k].drawCubit(signs[0]*wallSize, signs[1]*wallSize, signs[2]*wallSize);
          
        }
      }
    }
    
    popMatrix();
    
  }
  
  void scramble(String scramble) {
    
    int curr = 0;
    
    while(curr < scramble.length()) {
      
      if(curr < scramble.length()-1 && scramble.charAt(curr+1) == '\'') {
        this.rotateSide(scramble.substring(curr, curr+2));
        curr += 2;
      } else {
        this.rotateSide(scramble.substring(curr,curr+1));
        curr++;
      }
            
    }
    
  }
  
  String genScramble(int turns) {
    String[] rotations = {"f", "f'", "b", "b'", "l", "l'", "r", "r'", "u", "u'", "d", "d'"};
    String scramble = "";
    
    for(int i = 0; i < turns; i++)
      scramble += rotations[ floor( random( rotations.length ) ) ];
    
    return scramble;
    //this.scramble(scramble);
    
  }
  
  void rotateSide(String rotation) {
   
    Cubit temp;
    switch (rotation.toLowerCase()) {
      case "f":
        //move ActualCubes
        temp = cubits[0][0][1];
        cubits[0][0][1] = cubits[0][1][1];
        cubits[0][1][1] = cubits[1][1][1];
        cubits[1][1][1] = cubits[1][0][1];
        cubits[1][0][1] = temp;
      
       cubits[0][0][1].addRotation(0,0,-PI/2);
       cubits[0][1][1].addRotation(0,0,-PI/2);
       cubits[1][1][1].addRotation(0,0,-PI/2);
       cubits[1][0][1].addRotation(0,0,-PI/2);
        break;
        
      case "f'":
        temp = cubits[1][0][1];
        cubits[1][0][1] = cubits[1][1][1];
        cubits[1][1][1] = cubits[0][1][1];
        cubits[0][1][1] = cubits[0][0][1];
        cubits[0][0][1] = temp;
      
        cubits[0][0][1].addRotation(0,0,PI/2);
        cubits[0][1][1].addRotation(0,0,PI/2);
        cubits[1][1][1].addRotation(0,0,PI/2);
        cubits[1][0][1].addRotation(0,0,PI/2);
        break;
        
      case "b":
        temp = cubits[0][0][0];
        cubits[0][0][0] = cubits[1][0][0];
        cubits[1][0][0] = cubits[1][1][0];
        cubits[1][1][0] = cubits[0][1][0];
        cubits[0][1][0] = temp;
      
        cubits[0][0][0].addRotation(0,0,PI/2);
        cubits[1][0][0].addRotation(0,0,PI/2);
        cubits[1][1][0].addRotation(0,0,PI/2);
        cubits[0][1][0].addRotation(0,0,PI/2);
        break;
        
      case "b'":
        temp = cubits[0][1][0];
        cubits[0][1][0] = cubits[1][1][0];
        cubits[1][1][0] = cubits[1][0][0];
        cubits[1][0][0] = cubits[0][0][0];
        cubits[0][0][0] = temp;
      
        cubits[0][0][0].addRotation(0,0,-PI/2);
        cubits[1][0][0].addRotation(0,0,-PI/2);
        cubits[1][1][0].addRotation(0,0,-PI/2);
        cubits[0][1][0].addRotation(0,0,-PI/2);
        break;
        
      case "l":
      
        temp = cubits[0][0][0];
        cubits[0][0][0] = cubits[0][1][0];
        cubits[0][1][0] = cubits[0][1][1];
        cubits[0][1][1] = cubits[0][0][1];
        cubits[0][0][1] = temp;
        
        cubits[0][0][0].addRotation(PI/2,0,0);
        cubits[0][1][0].addRotation(PI/2,0,0);
        cubits[0][1][1].addRotation(PI/2,0,0);
        cubits[0][0][1].addRotation(PI/2,0,0);
        break;
        
      case "l'":
        temp = cubits[0][0][1];
        cubits[0][0][1] = cubits[0][1][1];
        cubits[0][1][1] = cubits[0][1][0];
        cubits[0][1][0] = cubits[0][0][0];
        cubits[0][0][0] = temp;
      
        cubits[0][0][1].addRotation(-PI/2,0,0);
        cubits[0][1][1].addRotation(-PI/2,0,0);
        cubits[0][1][0].addRotation(-PI/2,0,0);
        cubits[0][0][0].addRotation(-PI/2,0,0);
        break;
        
      case "r":
        temp = cubits[1][0][0];
        cubits[1][0][0] = cubits[1][0][1];
        cubits[1][0][1] = cubits[1][1][1];
        cubits[1][1][1] = cubits[1][1][0];
        cubits[1][1][0] = temp;
      
        cubits[1][0][0].addRotation(-PI/2,0,0);
        cubits[1][0][1].addRotation(-PI/2,0,0);
        cubits[1][1][1].addRotation(-PI/2,0,0);
        cubits[1][1][0].addRotation(-PI/2,0,0);
        break;
        
      case "r'":
        temp = cubits[1][1][0];
        cubits[1][1][0] = cubits[1][1][1];
        cubits[1][1][1] = cubits[1][0][1];
        cubits[1][0][1] = cubits[1][0][0];
        cubits[1][0][0] = temp;
      
        cubits[1][1][0].addRotation(PI/2,0,0);
        cubits[1][1][1].addRotation(PI/2,0,0);
        cubits[1][0][1].addRotation(PI/2,0,0);
        cubits[1][0][0].addRotation(PI/2,0,0);
        break;
        
        case "u":
        temp = cubits[0][0][0];
        cubits[0][0][0] = cubits[0][0][1];
        cubits[0][0][1] = cubits[1][0][1];
        cubits[1][0][1] = cubits[1][0][0];
        cubits[1][0][0] = temp;
      
        cubits[0][0][0].addRotation(0,-PI/2,0);
        cubits[0][0][1].addRotation(0,-PI/2,0);
        cubits[1][0][1].addRotation(0,-PI/2,0);
        cubits[1][0][0].addRotation(0,-PI/2,0);
        break;
        
      case "u'":
        temp = cubits[1][0][0];
        cubits[1][0][0] = cubits[1][0][1];
        cubits[1][0][1] = cubits[0][0][1];
        cubits[0][0][1] = cubits[0][0][0];
        cubits[0][0][0] = temp;
      
        cubits[0][0][0].addRotation(0,PI/2,0);
        cubits[0][0][1].addRotation(0,PI/2,0);
        cubits[1][0][1].addRotation(0,PI/2,0);
        cubits[1][0][0].addRotation(0,PI/2,0);
        break;
        
        case "d":
        temp = cubits[1][1][0];
        cubits[1][1][0] = cubits[1][1][1];
        cubits[1][1][1] = cubits[0][1][1];
        cubits[0][1][1] = cubits[0][1][0];
        cubits[0][1][0] = temp;
      
        cubits[0][1][0].addRotation(0,PI/2,0);
        cubits[0][1][1].addRotation(0,PI/2,0);
        cubits[1][1][1].addRotation(0,PI/2,0);
        cubits[1][1][0].addRotation(0,PI/2,0);
        break;
        
      case "d'":
        temp = cubits[0][1][0];
        cubits[0][1][0] = cubits[0][1][1];
        cubits[0][1][1] = cubits[1][1][1];
        cubits[1][1][1] = cubits[1][1][0];
        cubits[1][1][0] = temp;
      
        cubits[0][1][0].addRotation(0,-PI/2,0);
        cubits[0][1][1].addRotation(0,-PI/2,0);
        cubits[1][1][1].addRotation(0,-PI/2,0);
        cubits[1][1][0].addRotation(0,-PI/2,0);
        break;
    }
    
  }
  
}
