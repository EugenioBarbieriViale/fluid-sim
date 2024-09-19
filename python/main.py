import pygame, sys
import numpy as np
from random import randint

pygame.init()
clock = pygame.time.Clock()

X, Y = 1000, 800

screen = pygame.display.set_mode([X,Y])

N = 10
n_vels = 5
vmax = 100

def init_vels():
    vels = []
    for i in range(N):
        m = randint(-n_vels, n_vels) * vmax
        vel = pygame.math.Vector2(m)
        vels.append(vel)
    return vels

def init_pos():



while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    screen.fill((0,0,0))



    pygame.display.flip()
    clock.tick(60)
    pygame.display.update()

pygame.quit()
