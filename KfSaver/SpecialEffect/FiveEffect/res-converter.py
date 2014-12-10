
import re

def converter(verbose_file_name, tidy_file_name):
	vfile = open(verbose_file_name, "r")
	tfile = open(tidy_file_name, "w")
	for line in vfile:
		line = re.compile(r"#.*$").sub("", line)
		line = re.compile(r"[ \t\r\n]*", re.S).sub("", line)
		if line.upper() == "END":
			tfile.write("\n")
		elif len(line) > 0:
			tfile.write(line)
	vfile.close()
	tfile.close()


############################################################
############################################################


def converter2(layout_file_name, moves_file_name):
	lfile = open(layout_file_name, "r")
	mfile = open(moves_file_name, "w")

	column_line, layout_line, error_next = range(3)
	state = column_line
	line_num = 0
	for line in lfile:
		line_num += 1
		line = line.upper()

		line = re.compile(r"\s*(#.*)?$").sub("", line)	# remove tailing space and comment
		if len(line) == 0:	# empty line or comment line
			continue
		if " " in line:
			state = error_next
			print "Line %d: Error, space in line" % line_num
			continue


		dribs = line.split("\t")
		if state == error_next:
			if dribs[0] != "COL":
				continue
			state = column_line

		if state == layout_line:
			if dribs[0] == "COL":
				end_moves()
				state = column_line
			else:
				if not (len(dribs[0])==1 and ord('A') <= ord(dribs[0]) <= ord('Z')):
					state = error_next
					print "Line %d: expect Layout line, but got \"%s\"" % (line_num, line)
					continue
				parse_moves(dribs)

		if state == column_line:
			if dribs[0] != "COL":
				state == error_next
				print "Line %d: expect Column line, but got \"%s\"" % (line_num, line)
				continue
			state = layout_line
			new_moves(dribs)

	if state == layout_line:
		end_moves()

	write_games(mfile)
	lfile.close()
	mfile.close()

class game:
	def __init__(self):
		self.x_offset = None
		self.lines = []
		self.pos = {}

		self.valid = True

	def parse_column_line(self, dribs):
		if not self.valid:
			return False

		assert(dribs[0] == "COL")

		for drib in dribs[1:]:
			if len(drib) == 0 or re.compile(r"^\d+$").match(drib) is not None:
				pass # valid field
			else:
				print "drib is not valid:", drib
				self.valid = False
				return False

		offset = 0
		for drib in dribs[1:]:
			if len(drib) > 0:
				self.x_offset = int(drib) - offset
				self.lines.append(dribs)
				return True
			offset += 1
		self.valid = False
		return False

	def parse_move_line(self, dribs):
		if not self.valid:
			return False

		assert(len(dribs[0])==1 and ord('A')<=ord(dribs[0])<=ord('O'))

		for drib in dribs[1:]:
			if len(drib) == 0 or re.compile(r"^\d+$").match(drib) is not None:
				pass # valid field
			else:
				print "drib is not valid:", drib
				self.valid = False
				return False

		row = ord(dribs[0]) - ord('A') + 1
		col = 0
		for drib in dribs[1:]:
			if len(drib) > 0:
				move = int(drib)
				if self.pos.has_key(move):
					print "already have step %d..." % move
					self.valid = False
					return False
				self.pos[move] = (col + self.x_offset, row)
			col += 1
		self.lines.append(dribs)
		return True

	def verify_all(self):
		if not self.valid:
			return False

		pos = self.pos.keys(); pos.sort()
		if range(1, len(pos)+1) != pos:
			print "steps are not continual..."
			self.valid = False
			return False
		if self.pos[1] != (8, 8):
			print "the first step is not in (8, 8)"
			self.valid = False
			return False
		for x, y in self.pos.values():
			if not (1<=x<=15 and 1<=y<=15):
				print "step (%d, %d)?" % (x, y)
				self.valid = False
				return False
		return True

	def write_to(self, file):
		if not self.valid:
			return False

#		file.write("#\n")
#		for dribs in self.lines:
#			line = "# %s" % "\t".join(dribs)
#			file.write(line + "\n")
#		for step in range(1, len(self.pos)+1):
#			x, y = self.pos[step]
#			file.write("%s%d" % ( chr(y+ord('A')-1), x ))
#		file.write("\n")
		for step in range(1, len(self.pos)+1):
			x, y = self.pos[step]
			file.write(chr(x*16+y))
		file.write("\0")
#		seq = [ chr(x*16+y) for x, y in ]
#		data = "".join(seq) + "\0"
#		file.write(data)
#		print data

games = []
cur_game = None

def new_moves(dribs):
	global games, cur_game
#	print "new_moves"
	cur_game = game()
	cur_game.parse_column_line(dribs)

def parse_moves(dribs):
	global games, cur_game
#	print "parse_moves"
	cur_game.parse_move_line(dribs)

def end_moves():
	global games, cur_game
#	print "end_moves"
	if cur_game.verify_all():
		games.append(cur_game)
	else:
		print "game is not valid and will be skipped..."
	cur_game = None

def write_games(mfile):
	global games, cur_game
#	print "write_games"
	for g in games:
#		print "one game..."
		g.write_to(mfile)

################################################

import sys
import os

if __name__ == '__main__':
	my_dir = os.path.dirname(os.path.realpath(sys.argv[0]))
	converter2(my_dir + r"\five.txt", my_dir + r"\..\..\res\five.dat")
