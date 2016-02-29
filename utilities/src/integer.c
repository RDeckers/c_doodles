#include <utilities/integer.h>

int isqrt(int n){
  int sign = signOf(n);
  n = iabs(n);
  int cur = n, next = (n+1)/2;
  while(iabs(next-cur) > 1){
    cur = next;
    next = (cur + n/cur)/2;
  }
  return sign*next;
}

unsigned usqrt(unsigned x){ //fails on x= UINT_MAX, gives 0.
  unsigned cur = x, next = (x+1)/2;
  while(next-cur > 1){
    cur = next;
    next = (cur + x/cur)/2;
  }
  return cur;
}


void generatePrimes(unsigned *primes, unsigned N_PRIMES){
  unsigned int incr = 2, incrLoop = 2;
  unsigned seed_primes[] = {2,3,5,7,11,13};
  if(N_PRIMES < 6){//seed primes enough
    for(unsigned u = 0; u < N_PRIMES;u++)
    primes[u] = seed_primes[u];
    return;
  }
  primes[0] = seed_primes[0];
  primes[1] = seed_primes[1]; //2 en 3 nnodig, vanaf 5 kan
  primes[2] = seed_primes[2];
  primes[3] = seed_primes[3];
  primes[4] = seed_primes[4];
  primes[5] = seed_primes[5];
  unsigned MaxDiv = 4;
  unsigned stepsToNextMaxDiv = 9;
  unsigned stepsTaken = 1;
  unsigned currentPrime = 5;
  unsigned nextNumber=17;
  unsigned residue;
  unsigned i;
  unsigned max_prime_spot = 5;
  while(currentPrime < N_PRIMES  - 1)
  {
    incrLoop = 2;
    i = 5;
    do{
      residue = nextNumber%i;
      if(residue == 0)
      goto jumper;
      i+=incrLoop;
      incrLoop^= 6;
    }while(i<= primes[max_prime_spot]);
    primes[++currentPrime] = nextNumber;
    jumper:
    nextNumber += incr;
    stepsTaken +=incr;
    incr ^= 6;
    if(stepsTaken >= stepsToNextMaxDiv){
      stepsTaken -= stepsToNextMaxDiv;
      stepsToNextMaxDiv+=2;
      MaxDiv++;
      if(primes[max_prime_spot+1] >= MaxDiv)
      max_prime_spot++;
    }
  }
  return;
}
