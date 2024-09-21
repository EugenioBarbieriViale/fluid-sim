import pygame, sys
from gas import Gas

pygame.init()
clock = pygame.time.Clock()

X, Y = 1000, 800
FPS = 30

screen = pygame.display.set_mode([X,Y])

N = 1000
R = 5

n_vels = 5
vmax = 30

gas = Gas(N, R, n_vels, vmax, FPS)

gas.init(X, Y)

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    screen.fill((0,0,0))

    gas.run_simulation(screen, X, Y)

    pygame.display.flip()
    clock.tick(FPS)
    pygame.display.update()

pygame.quit()
