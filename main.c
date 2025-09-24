#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <raylib.h>

const int X = 1000;
const int Y =  800;
const int FPS = 60;

const int N =   50;
const float R = 10;

const float H =  50; // interaction radius
const float D_0 = 1; // rest density

const float K = 1; // stiffness constant
const float K_NEAR = 1; // stiffness constant

const float DAMPING = 0.8;
const Vector2 g = {0, 9.81};


typedef struct {
	Vector2 pos;
	Vector2 vel;
	float pressure;
} Particle;


float rand_float(float low, float high) {
    return ((float)rand() / (float)(RAND_MAX)) * fabs(low - high) + low;
}

Vector2 sum(Vector2 v, Vector2 w) {
    return (Vector2){v.x + w.x, v.y + w.y};
}

Vector2 diff(Vector2 v, Vector2 w) {
    return (Vector2){v.x - w.x, v.y - w.y};
}

Vector2 scalar_mult(Vector2 v, float s) {
	return (Vector2){v.x * s, v.y * s};
}

float length(Vector2 v) {
    return (float)sqrt(v.x*v.x + v.y*v.y);
}

Particle init_p() {
	Particle p = {
		p.pos = (Vector2){rand_float(R, X - R), rand_float(R, Y - R)},
		p.vel = (Vector2){0, 0},
		p.pressure = 0.f,
	};

	return p;
}

void dd_relaxation(Particle *p, Particle f[], float dt) {
	float density = 0.0;
	float near_density = 0.0;

	for (int j=0; j<N; j++) {
		float dist = length(diff(p->pos, f[j].pos));

		if (dist < H) {
			density += pow(1 - dist / H, 2);
			near_density += pow(1 - dist / H, 3);
		}
	}

	float pressure = K * (density - D_0);
    printf("%f\n", pressure);
    float near_pressure = K_NEAR * near_density;
                                          
    Vector2 d_pos = {0.f, 0.f};

	for (int j=0; j<N; j++) {
		Vector2 dist = diff(p->pos, f[j].pos);

        if (length(dist) < H) {
            float factor = pow(dt, 2) * (pressure * (1 - length(dist) / H) +
                    near_pressure * pow(1 - length(dist) / H, 2));

            Vector2 versor = scalar_mult(dist, 1.f / length(dist));
            Vector2 displacement = scalar_mult(versor, factor);

            f[j].pos = sum(f[j].pos, scalar_mult(displacement, 0.5f));
            d_pos = diff(d_pos, scalar_mult(displacement, 0.5f));
        }
    }

    p->pos = sum(p->pos, d_pos);
}

void borders(Particle *p, float dt) {
	if (p->pos.x + p->vel.x * dt <= R ||
        p->pos.x + p->vel.x * dt >= X - R) {

		p->vel.x *= -1 * DAMPING;
	}

	if (p->pos.y + p->vel.y * dt <= R ||
        p->pos.y + p->vel.y * dt >= Y - R) {

		p->vel.y *= -1 * DAMPING;
	}
}

void draw(Particle p) {
    DrawCircleV(p.pos, R, GRAY);
}

int main() {
    srand(time(NULL));

    InitWindow(X, Y, "Fluid Simulation");
    SetTargetFPS(FPS);

	Particle *f = malloc(N * sizeof(Particle));

	if (f) {
		for (int i=0; i<N; i++) {
			f[i] = init_p();
		}
	}

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

		for (int i=0; i<N; i++) {
            Particle *p = &f[i];

            p->vel = sum(scalar_mult(g, dt), p->vel); // gravity
            
            // viscosity

            Vector2 prev_pos = p->pos;
            p->pos = sum(scalar_mult(p->vel, dt), p->pos);

            // adjust springs
            // modify position according to springs
            // double density relaxation
            dd_relaxation(p, f, dt);
            // collision resolution

			borders(&f[i], dt);

            p->vel = scalar_mult(diff(p->pos, prev_pos), 1/dt);

            // printf("%f", f[i].pos.x);
			draw(f[i]);
		}

        EndDrawing();
    }

    CloseWindow();

	free(f);
    return 0;
}
