//Rubicks sube solver using genitic algorithm

ActualCube cube1, cube2;

ActualCube target;
int popMax;
float mutationRate;
GeneticAlgo genAlg;
int turns, restrictedTurns;
float wallSize = 75;
int totalGens = 0;

void setup() {
  
  size(900,900,P3D);
  frameRate(120);
  smooth(4);
  
  target = new ActualCube(2,2,2, wallSize);
  popMax = 1000;
  mutationRate = 0.2;
  turns = 50;
  restrictedTurns = 40;
  genAlg = new GeneticAlgo(target.x, target.y, target.x, turns, wallSize, target, mutationRate, popMax);
  genAlg.adjustRestrictedTurns(restrictedTurns);
  
  /*cube1 = new ActualCube(3,3,3, 30);
  cube2 = new ActualCube(3,3,3, 30);
  cube2.scramble("fulrfbd");
  
  //println(fitness(cube1,cube2));
  
  cube1.scramble(cube1.genScramble(20));
  cube2.scramble(cube2.genScramble(20));*/
  
 //target.scramble("f'r'f'u'ffr'u'rruuruufr'f'rrffu'r'f'r'u'ffrffu'ruuruffuufrf'rfrurrffur'uf'rurruur'u");
}


ActualCube currBestActualCube, prevBestActualCube;
int frameNum = 0, highestFit = 0, gensOfStagnation = 0;

void draw() {
  
  frameNum++;
  
  background(50);
  textSize(35);
  textAlign(CENTER);
  
  boolean rotationTesting = true;
  float radY = float(mouseX)/float(width) * 2*PI;
  float radX = float(mouseY)/float(height) * 2*PI;
  
  DNA best = genAlg.getBest();
  prevBestActualCube = currBestActualCube;
  currBestActualCube = best.cube;
  
  if(highestFit < best.fitness)
    highestFit = best.fitness;
  
  text("Best Curr Fitness: " + best.fitness + "\nNum of Turns: " + (turns-restrictedTurns) + "\nGoal Fitness: " + genAlg.perfectScore + "\nGeneration: " + totalGens, width/2, height/15);
  textSize(20);
  text("Best Solve:\n" + best.getGenes().substring(0,turns-restrictedTurns), width/2, height-height/9);
  
  best.cube.drawActualCube(width/2,height/1.8,0, (rotationTesting)?radX:0, (rotationTesting)?radY:0);
  
  if(frameNum >= 0 && !genAlg.finished()) {
    genAlg.naturalSelection();
    genAlg.generate();
    genAlg.calcFitness();
    
    totalGens++;
     
    if(currBestActualCube == prevBestActualCube) {
      gensOfStagnation++;
    
      if(gensOfStagnation >= 500) {
        
        if(restrictedTurns == 0) {
          turns = 25;
          restrictedTurns = 10;
          genAlg = new GeneticAlgo(target.x, target.y, target.x, turns, wallSize, target, mutationRate, popMax);
          genAlg.adjustRestrictedTurns(restrictedTurns);
        }
        
        gensOfStagnation = 0;
        genAlg.adjustRestrictedTurns(--restrictedTurns);
      }
    } else
      gensOfStagnation = 0;
      
    frameNum = 0;  
  }
  
  
  /*cube1.drawActualCube(300,450,0, (rotationTesting)?radX:0, (rotationTesting)?radY:0);
  cube2.drawActualCube(600,450,0, (rotationTesting)?radX:0, (rotationTesting)?radY:0);*/
  
}

void keyReleased() {
  
  /*if(key == ' ' && !genAlg.finished()) {
    genAlg.naturalSelection();
    genAlg.generate();
    genAlg.calcFitness();
  }*/
    
  
  if(key == 'l')
    target.rotateSide("l");
  else if(key == 'f')
    target.rotateSide("f");
  else if(key == 'b')
    target.rotateSide("b");
  else if(key == 'r')
    target.rotateSide("r");
  else if(key == 'd')
    target.rotateSide("d");
  else if(key == 'u')
    target.rotateSide("u");
    
  if(key == ';')
    target.rotateSide("l'");
  else if(key == 'g')
    target.rotateSide("f'");
  else if(key == 'n')
    target.rotateSide("b'");
  else if(key == 't')
    target.rotateSide("r'");
  else if(key == 's')
    target.rotateSide("d'");
  else if(key == 'i')
    target.rotateSide("u'");
  
}
