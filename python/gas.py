import pygame
import numpy as np
from random import random, choice

class Gas:
    def __init__(self, N, R, n_vels, vmax, FPS):
        self.N = N
        self.R = R

        self.n_vels = n_vels
        self.vmax = vmax

        self.shift = 30

        self.positions  = []
        self.vels = []

        self.possible_vels = [v * self.vmax for v in range(-self.n_vels, self.n_vels+1) if v != 0]

        self.dt = 1/FPS

    def init_pos(self, X, Y):
        for i in range(self.N):
            pos = pygame.math.Vector2()
            pos.x = random() * (X - self.shift * 2) + self.shift;
            pos.y = random() * (Y - self.shift * 2) + self.shift;
            
            self.positions.append(pos)

    def init_vels(self):
        self.prob_of_vel = [0 for i in range(self.n_vels)]

        for i in range(self.N):
            x = choice(self.possible_vels)
            y = choice(self.possible_vels)

            vel = pygame.math.Vector2(x,y)
            self.vels.append(vel)

    def init(self, X, Y):
        self.init_pos(X,Y)
        self.init_vels()

    def move(self, X, Y, i):
        if self.positions[i].x < self.R or self.positions[i].x > X - self.R:
            self.vels[i].x *= -1

        if self.positions[i].y < self.R or self.positions[i].y > Y - self.R:
            self.vels[i].y *= -1

        self.positions[i] += self.vels[i] * self.dt

    def draw(self, screen, i):
        pygame.draw.circle(screen, (200,200,200), self.positions[i], self.R)

    def run_simulation(self, screen, X, Y):
        vqm = 0

        for i in range(self.N):
            self.move(X, Y, i)
            self.draw(screen, i)

            vqm += self.vels[i].magnitude_squared()

        vqm = np.sqrt(vqm / self.N)
        print("Vqm:", round(vqm,2))
