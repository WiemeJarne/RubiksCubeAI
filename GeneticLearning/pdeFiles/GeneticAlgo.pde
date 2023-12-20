//This is the population

class GeneticAlgo {

  //--------------------------Genetic Algorithm Code Below (Population Class)-------------------------------//
  
  DNA prevBest;
  float mutationRate;
  DNA[] population;
  ArrayList<DNA> matingPool;
  ActualCube target;
  String ogScramble;
  int generations;
  boolean finished;
  int fitnessPow = 3;
  int perfectScore;  // = int(pow(26,fitnessPow)); //26 for the # of pieces in a cube minus 1                 //54 for the # of outward facing sides on a 3x3x3 cube (9*6 = 54)
  
  GeneticAlgo(int x, int y, int z, int turns, float wallSize, ActualCube target, float mutationRate, int size) {
    this.ogScramble = "f'r'f'u'ffr'u'rruuruufr'f'rrffu'r'f'r'u'ffrffu'ruuruffuufrf'rfrurrffur'uf'rurruur'u";
    //this.ogScramble = (new ActualCube(x,y,z,wallSize)).genScramble(turns);
    this.target = target;
    this.mutationRate = mutationRate;
    population = new DNA[size];
    
    for(int i = 0; i < population.length; i++) {
      population[i] = new DNA(x, y, z, turns, wallSize, ogScramble);
    }
    
    calcFitness();
    matingPool = new ArrayList<DNA>();
    finished = false;
    generations = 0;
    
    DNA perf = new DNA(x,y,z,turns,wallSize,ogScramble);
    perf.cube = new ActualCube(x,y,z,wallSize);
    perf.fitness(target);
    perfectScore = perf.fitness;

  }
  
  //calculate fitness of all the members of the pop
  void calcFitness() {
    
    for(int i = 0; i < population.length; i++) {
      population[i].fitness(target);
    }
    
  }
  
  //fill the mating pool for new generation
  void naturalSelection() {
    
    matingPool.clear();
    
    float maxFitness = 0;
    for(int i = 0; i < population.length; i++)
      if(population[i].fitness > maxFitness)
        maxFitness = population[i].fitness;
        
    for(int i = 0; i < population.length; i++) {
      
      float fitness = map(population[i].fitness, 0, maxFitness, 0, 1);
      int amt = int(fitness)*100; //arbitrary multiplier of 100
        //println(int(fitness));
      for(int j = 0; j < amt; j++)
        matingPool.add(population[i]);
      
    }
    
  }
  
  //make new generation
  void generate() {
    
    DNA best = this.getBest();
    population[0] = best;
    //best.fitness(target);
    population[0] = best;
    //int x, int y, int z, int turns, float wallSize, String ogScramble, String genes, ActualCube target
    DNA bestMut = new DNA(target.x, target.y, target.z, turns, target.wallSize, ogScramble, new String(best.genes), target);
    bestMut.mutate(mutationRate);
    population[1] = bestMut;
        
    for(int i = 2; i < population.length; i++) {
      
      DNA partnerA = matingPool.get( int(random(matingPool.size())) );
      DNA partnerB = matingPool.get( int(random(matingPool.size())) );
      DNA child = partnerA.crossover(partnerB);
      child.mutate(mutationRate);
      population[i] = child;
      
    }
    
    population[0].fitness(target);
    population[1].fitness(target);
    
    generations++;
    
  }
  
  //returns the cube with the best fitness score
  DNA getBest() {
    int record = 0, index = 0;
    
    for(int i = 0; i < population.length; i++) {
      if(population[i].fitness > record) {
        index = i;
        record = population[i].fitness;
      }
    }
    
    if(record == perfectScore)
      finished = true;
    
    return population[index];
    
  }
  
  void adjustRestrictedTurns(int newRestrictedTurns) {
    for(int i = 0; i < population.length; i++) {
      population[i].restrictedTurns = newRestrictedTurns;
    }
  }
  
  boolean finished() {
    return this.finished;
  }
  
  int getGenerations() {
    return this.generations;
  }
  
  float getAverageFitness() {
    float total = 0;
    for (int i = 0; i < population.length; i++) {
      total += population[i].fitness;
    }
    return total / (population.length);
  }
    
  
}
