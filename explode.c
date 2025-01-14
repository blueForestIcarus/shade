//stolen from https://codegolf.stackexchange.com/questions/24462/display-the-explosion-of-a-star-in-ascii-art

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define NUM_FRAMES 150
#define NUM_BLOBS 400
#define PERSPECTIVE 50.0

typedef struct {
  double x,y,z;
} spaceblob;

double prng() {
  static long long s=1;
  s = s * 1488248101 + 981577151;
  return ((s % 65536) - 32768) / 32768.0;
}

int main() {
  char *frames[NUM_FRAMES], *p;
  int i,j,x,y,z,v,rows,cols,ith,i0;
  float radius, theta;
  int maxx,minx,maxy,miny,delay=1E6;
  double bx,by,bz,br,r,th,t;
  spaceblob *blobs;

  /* Initialize ncurses and get window dimensions */
  initscr();
  erase();
  refresh();
  getmaxyx(stdscr,rows,cols);
  minx = -cols / 2;
  maxx = cols+minx-1;
  miny = -rows / 2;
  maxy = rows+miny-1;

  /* Generate random blob coordinates */
  blobs = malloc(NUM_BLOBS * sizeof(spaceblob));
  for (i=0; i<NUM_BLOBS; i++) {
    bx = prng();
    by = prng();
    bz = prng();
    br = sqrt(bx*bx + by*by + bz*bz);
    blobs[i].x = (bx / br) * (1.3 + 0.2 * prng());
    blobs[i].y = (0.5 * by / br) * (1.3 + 0.2 * prng());;
    blobs[i].z = (bz / br) * (1.3 + 0.2 * prng());;
  }

  /* Generate animation frames */
  for (i=0; i<NUM_FRAMES; i++) {
    t = (1. * i) / NUM_FRAMES;
    p = frames[i] = malloc(cols * rows + 1);
    for (y=miny; y<=maxy; y++) {
      for (x=minx; x<=maxx; x++) {

        /* Otherwise show blast wave */
		theta = atan2(y*2.+0.01,x+0.01);
		radius = sqrt(x*x + 4*y*y);
        r = radius * (.7 + (prng()*.1) * cos(16. * theta+i/10.));
        ith = 32 + th * 32. * M_1_PI;
        v = i - r;
        if (v<7) *p++ = (i<19)?"%@W#H=+~-:."[i-1]:' '; /* initial flash */
        else if (v<20) *p++ = " .:!HIOMW#%$&@08O=+-"[v];
        else *p++=' ';
      }
    }

    /* Add blobs with perspective effect */
    if (i>7) {
      i0 = i;
      for (j=0; j<NUM_BLOBS; j++) {
        bx = blobs[j].x * i0;
        by = blobs[j].y * i0;
        bz = blobs[j].z * i0;
        if (bz<5-PERSPECTIVE || bz>PERSPECTIVE) continue;
        x = cols / 2 + bx * PERSPECTIVE / (bz+PERSPECTIVE);
        y = rows / 2 + by * PERSPECTIVE / (bz+PERSPECTIVE);
        if (x>=0 && x<cols && y>=0 && y<rows)
          frames[i][x+y*cols] = (bz>40) ? '.' : (bz>-20) ? 'o' : '@';
      }
    }

    /* Terminate the text string for this frame */
    *p = '\0';
  }

  /* Now play back the frames in sequence */
  curs_set(0); /* hide text cursor (supposedly) */
  for (i=0; i<NUM_FRAMES; i++) {
    erase();
    mvaddstr(0,0,frames[i]);
    refresh();
    usleep(delay);
    delay=30000; /* Change to 30fps after first frame */
  }

 for (i=NUM_FRAMES-1; i>=0; i--) {
    erase();
    mvaddstr(0,0,frames[i]);
    refresh();
    usleep(delay);
    delay=30000; /* Change to 30fps after first frame */
  }

  curs_set(1); /* unhide cursor */
  endwin(); /* Exit ncurses */
  return 0;
}

int old() {
  char *frames[NUM_FRAMES], *p;
  int i,j,x,y,z,v,rows,cols,ith,i0;
  int maxx,minx,maxy,miny,delay=1E6;
  double bx,by,bz,br,r,th,t;
  spaceblob *blobs;

  /* Initialize ncurses and get window dimensions */
  initscr();
  getmaxyx(stdscr,rows,cols);
  minx = -cols / 2;
  maxx = cols+minx-1;
  miny = -rows / 2;
  maxy = rows+miny-1;

  /* Generate random blob coordinates */
  blobs = malloc(NUM_BLOBS * sizeof(spaceblob));
  for (i=0; i<NUM_BLOBS; i++) {
    bx = prng();
    by = prng();
    bz = prng();
    br = sqrt(bx*bx + by*by + bz*bz);
    blobs[i].x = (bx / br) * (1.3 + 0.2 * prng());
    blobs[i].y = (0.5 * by / br) * (1.3 + 0.2 * prng());;
    blobs[i].z = (bz / br) * (1.3 + 0.2 * prng());;
  }

  /* Generate animation frames */
  for (i=0; i<NUM_FRAMES; i++) {
    t = (1. * i) / NUM_FRAMES;
    p = frames[i] = malloc(cols * rows + 1);
    for (y=miny; y<=maxy; y++) {
      for (x=minx; x<=maxx; x++) {

        /* Show a single '*' in first frame */
        if (i==0) {
          *p++ = (x==0 && y==0) ? '*' : ' ';
          continue;
        }

        /* Show expanding star in next 7 frames */
        if (i<8) {
          r = sqrt(x*x + 4*y*y);
          *p++ = (r < i*2) ? '@' : ' ';
          continue;
        }

        /* Otherwise show blast wave */
        r = sqrt(x*x + 4*y*y) * (0.5 + (prng()/3.0)*cos(16.*atan2(y*2.+0.01,x+0.01))*.3);
        ith = 32 + th * 32. * M_1_PI;
        v = i - r - 7;
        if (v<0) *p++ = (i<19)?"%@W#H=+~-:."[i-8]:' '; /* initial flash */
        else if (v<20) *p++ = " .:!HIOMW#%$&@08O=+-"[v];
        else *p++=' ';
      }
    }

    /* Add blobs with perspective effect */
    if (i>6) {
      i0 = i-6;
      for (j=0; j<NUM_BLOBS; j++) {
        bx = blobs[j].x * i0;
        by = blobs[j].y * i0;
        bz = blobs[j].z * i0;
        if (bz<5-PERSPECTIVE || bz>PERSPECTIVE) continue;
        x = cols / 2 + bx * PERSPECTIVE / (bz+PERSPECTIVE);
        y = rows / 2 + by * PERSPECTIVE / (bz+PERSPECTIVE);
        if (x>=0 && x<cols && y>=0 && y<rows)
          frames[i][x+y*cols] = (bz>40) ? '.' : (bz>-20) ? 'o' : '@';
      }
    }

    /* Terminate the text string for this frame */
    *p = '\0';
  }

  /* Now play back the frames in sequence */
  curs_set(0); /* hide text cursor (supposedly) */
  for (i=0; i<NUM_FRAMES; i++) {
    erase();
    mvaddstr(0,0,frames[i]);
    refresh();
    usleep(delay);
    delay=33333; /* Change to 30fps after first frame */
  } 

  curs_set(1); /* unhide cursor */
  endwin(); /* Exit ncurses */
  return 0;
}
