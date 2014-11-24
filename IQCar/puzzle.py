#!/usr/bin/env python

from collections import deque
import copy

def log(msg, *args):
    pass
#    if args:
#        msg = msg % args
#    print msg

class Car:
    def __init__(self, name, direction, length):
        self.name = name
        self.direction = direction
        self.length = length

class Node:
    size = 6
    vertical = 0
    horizontal = 1
    offset_v = (0, 1)
    offset_h = (1, 0)
    offsets = [offset_v, offset_h]

    def __init__(self):
        self.cars = {}
        self.board = [[None]*self.size for x in xrange(self.size)]
        self.king = None

    def expand(self):
        for org, car in self.cars.items():
            x, y = org
            ox, oy = self.offsets[car.direction]

            bx, by = x - ox, y - oy
            length = car.length
            if bx >= 0 and by >= 0 and self.board[by][bx] is None:
                back = self._clone()
                pos = (bx, by)
                for i in range(length):
                    back.board[by + oy*i][bx + ox*i] = pos
                back.board[by + oy*length][bx + ox*length] = None
                back.cars[pos] = back.cars[org]
                del back.cars[org]
                if org == back.king:
                    back.king = pos
                yield back

            fx, fy = x + ox * length, y + oy * length
            if fx < self.size and fy < self.size and self.board[fy][fx] is None:
                forward = self._clone()
                pos = (x + ox, y + oy)
                for i in range(1, length+1):
                    forward.board[y + oy*i][x + ox*i] = pos
                forward.board[y][x] = None
                forward.cars[pos] = forward.cars[org]
                del forward.cars[org]
                if org == forward.king:
                    forward.king = pos
                yield forward
        return

    def succeed(self):
        x, y = self.king
        car = self.cars[self.king]
        assert car.direction == self.horizontal
        assert y == 3 and 0 <= x < self.size
        assert x + car.length <= self.size
        return x + car.length == self.size

    @classmethod
    def generate(cls, level=100):
        node = Node()
        if level == 100:
            node._place(0, 3, cls.horizontal, 2, 'king')
            node._place(0, 0, cls.vertical, 2, None)
            node._place(0, 4, cls.vertical, 2, None)
            node._place(1, 4, cls.vertical, 2, None)
            node._place(2, 2, cls.vertical, 3, None)
            node._place(3, 1, cls.vertical, 2, None)
            node._place(5, 0, cls.vertical, 3, None)
            node._place(0, 2, cls.horizontal, 2, None)
            node._place(1, 0, cls.horizontal, 3, None)
            node._place(1, 1, cls.horizontal, 2, None)
            node._place(2, 5, cls.horizontal, 2, None)
            node._place(4, 4, cls.horizontal, 2, None)
            node._place(4, 5, cls.horizontal, 2, None)
        else:
            # level 1
            node._place(0, 3, cls.horizontal, 2, 'red')
            node._place(0, 0, cls.vertical, 3, 'green')
            node._place(1, 2, cls.horizontal, 3, 'pink')
            node._place(3, 3, cls.vertical, 3, 'purple')
        return node

    def __cmp__(self, another):
        return cmp(self.board, another.board)

    def __str__(self):
        colors = 'ABCDEFGHIJKLMN'
        idx = 0
        mapping = {}
        out = []
        for line in self.board:
            oline = []
            for cell in line:
                if cell is None:
                    oline.append(' ')
                elif mapping.has_key(cell):
                    oline.append(mapping[cell])
                else:
                    c = colors[idx]
                    idx += 1
                    mapping[cell] = c
                    oline.append(c)
            out.append(''.join(oline))
        out.reverse()
        return '\n'.join(out)

    def _place(self, x, y, direction, length, name):
        assert 0 <= x < self.size
        assert 0 <= y < self.size
        assert direction == self.vertical or direction == self.horizontal
        assert length == 2 or length == 3
        if direction == self.vertical:
            assert y + length <= self.size
        else:
            assert x + length <= self.size

        car = Car(name, direction, length)
        ox, oy = self.offsets[direction]
        for i in range(length):
            if self.board[y + oy*i][x + ox*i] is not None:
                raise Unplaceable
        pos = (x, y)
        for i in range(length):
            self.board[y + oy*i][x + ox*i] = pos
        self.cars[pos] = car
        if self.king is None:
            assert y == 3
            assert direction == self.horizontal
            assert x + length < self.size
            self.king = pos

    def _clone(self):
        clone = Node()
        clone.cars = copy.copy(self.cars)
        clone.board = [line[:] for line in self.board]
        clone.king = self.king
        return clone

class Resolver:
    @classmethod
    def resolve(cls, init_node):
        init_node.parent = None
        init_node.depth = 0
        if init_node.succeed():
            return cls._compose_route(init_node)

        wait_queue = deque()
        wait_queue.append(init_node)

        all_nodes = [init_node]

        while len(wait_queue) > 0:
            node = wait_queue.popleft()

            log("Expanding %d %s", node.depth, '=' * 60)
            log("%s", node)
            for child in node.expand():
                child.parent = node
                child.depth = node.depth + 1
                if child.succeed():
                    return cls._compose_route(child)

                for v in all_nodes:
                    if child == v:
                        break
                else:
                    log("======")
                    log("%s", child)
                    wait_queue.append(child)
                    all_nodes.append(child)
        return None

    @classmethod
    def _compose_route(cls, last):
        node = last
        route = [node]
        while node.parent is not None:
            node = node.parent
            route.append(node)
        route.reverse()
        return route

def main():
    init = Node.generate()
    route = Resolver.resolve(init)
    for idx, node in enumerate(route):
        print '======', idx
        print node

if __name__ == '__main__':
    main()

