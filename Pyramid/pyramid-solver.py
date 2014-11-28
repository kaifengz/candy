
import wx
import random
import time

def alert(parent, message, caption = 'Pyramid Solver'):
	dlg = wx.MessageDialog(parent, message, caption, wx.OK | wx.ICON_INFORMATION)
	dlg.ShowModal()
	dlg.Destroy()

class MyPanel(wx.Panel):
	colors = {
			'A': wx.Color(128, 128, 255),
			'B': wx.Color(255, 128, 128),
			'C': wx.Color(128, 255, 128),
			'D': wx.Color(  0,   0, 128),
			'E': wx.Color(  0, 128,   0),
			'F': wx.Color(128,   0,   0),
			'G': wx.Color(128, 128,   0),
			'H': wx.Color(  0, 128, 128),
			'I': wx.Color(128,   0, 128),
			'J': wx.Color(  0, 255, 128),
			'K': wx.Color(255, 128,   0),
			'L': wx.Color(128,   0, 255),
			}

	def __init__(self, *args, **kwargs):
		super(MyPanel, self).__init__(*args, **kwargs)
		self.problem = Problem()
		self.problem.shuffle()

		self.moving  = None
		self.moved   = False
		self.xmove   = None
		self.ymove   = None
		self.xoffset = None
		self.yoffset = None

		self.Bind(wx.EVT_PAINT,       self.OnPaint)
		self.Bind(wx.EVT_LEFT_DOWN,   self.OnLeftDown)
		self.Bind(wx.EVT_LEFT_UP,     self.OnLeftUp)
		self.Bind(wx.EVT_RIGHT_UP,    self.OnRightUp)
		self.Bind(wx.EVT_MOTION,      self.OnMouseMove)
		self.Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDClick)

	_xmargin, _ymargin = 20, 20
	_radius = 12
	_offset = 30
	_sqrt2 = 1.4142
	_board_size = int(_offset * 11.5 / _sqrt2)
	_x0, _y0 = _xmargin + _board_size, _ymargin + _offset

	_columns, _rows = 3, 4
	assert _columns * _rows == 12

	_x1, _y1 = _xmargin, _ymargin + _board_size + 20
	_cell_width = _board_size * 2 / _columns
	_cell_height = int(_cell_width / _sqrt2)

	@classmethod
	def _get_coord1(cls, level, index):
		x = cls._x0 + int((index - level) * cls._offset / cls._sqrt2)
		y = cls._y0 + int((index + level) * cls._offset / cls._sqrt2)
		return x, y

	@classmethod
	def _get_coord2(cls, row, column, level, index):
		x2 = cls._x1 + column * cls._cell_width + cls._cell_width / 2
		y2 = cls._y1 + row * cls._cell_height + cls._offset / 2
		x = x2 + int((index - level) * cls._offset / cls._sqrt2)
		y = y2 + int((index + level) * cls._offset / cls._sqrt2)
		return x, y

	def _get_coord3(self, level, index):
		x = self.xmove - self.xoffset + int((index - level) * self._offset / self._sqrt2)
		y = self.ymove - self.yoffset + int((index + level) * self._offset / self._sqrt2)
		return x, y

	def OnPaint(self, evt):
		_xmargin, _ymargin, _radius, _board_size = \
				self._xmargin, self._ymargin, self._radius, self._board_size
		_columns, _rows, _x1, _y1, _cell_width, _cell_height = \
				self._columns, self._rows, self._x1, self._y1, self._cell_width, self._cell_height

		dc = wx.BufferedPaintDC(self)

		white_pen = wx.Pen(wx.Color(255, 255, 255))
		black_pen = wx.Pen(wx.Color(0, 0, 0))
		white_brush = wx.Brush(wx.Color(255, 255, 255))

		dc.SetPen(white_pen)
		dc.SetBrush(white_brush)
		dc.DrawRectangle(0, 0, *self.GetSizeTuple())

		dc.SetPen(black_pen)
		dc.DrawLine(_xmargin, _ymargin + _board_size, _xmargin + _board_size, _ymargin)
		dc.DrawLine(_xmargin + _board_size, _ymargin, _xmargin + _board_size * 2, _ymargin + _board_size)
		dc.DrawLine(_xmargin + _board_size * 2, _ymargin + _board_size, _xmargin, _ymargin + _board_size)

		for row in xrange(_rows+1):
			dc.DrawLine(_x1, _y1 + _cell_height * row, _x1 + _cell_width * _columns, _y1 + _cell_height * row)
		for column in xrange(_columns+1):
			dc.DrawLine(_x1 + _cell_width * column, _y1, _x1 + _cell_width * column, _y1 + _cell_height * _rows)

		board = self.problem.board
		for level in xrange(10):
			length = 10 - level
			for index in xrange(length):
				c = board[level][index]
				if c == ' ':
					dc.SetPen(black_pen)
					dc.SetBrush(white_brush)
				else:
					color = self.colors[c]
					dc.SetPen(wx.Pen(color))
					dc.SetBrush(wx.Brush(color))
				x, y = self._get_coord1(level, index)
				dc.DrawCircle(x, y, _radius)

		pieces = self.problem.pieces
		for row in xrange(_rows):
			for column in xrange(_columns):
				piece = pieces[row * _columns + column]
				if piece.placed:
					continue
				if piece is self.moving and self.moved:
					continue
				color = self.colors[piece.color]
				dc.SetPen(wx.Pen(color))
				dc.SetBrush(wx.Brush(color))
				for level in xrange(piece.height):
					for index in xrange(piece.width):
						if piece.shape[level][index] == ' ':
							continue
						x, y = self._get_coord2(row, column, level, index)
						dc.DrawCircle(x, y, _radius)

		if self.moving and self.moved:
			piece = self.moving
			color = self.colors[piece.color]
			dc.SetPen(wx.Pen(color))
			dc.SetBrush(wx.Brush(color))
			for level in xrange(piece.height):
				for index in xrange(piece.width):
					if piece.shape[level][index] == ' ':
						continue
					x, y = self._get_coord3(level, index)
					dc.DrawCircle(x, y, _radius)

	def _rget_which1(self, evt, xoffset=0, yoffset=0):
		x = evt.X - xoffset - self._x0
		y = evt.Y - yoffset - self._y0
		level = int((y - x) / self._sqrt2 / self._offset + 0.5)
		index = int((y + x) / self._sqrt2 / self._offset + 0.5)
		return level, index

	@classmethod
	def _rget_which2(cls, evt):
		row    = (evt.Y - cls._y1) / cls._cell_height
		column = (evt.X - cls._x1) / cls._cell_width
		which  = row * cls._columns + column
		return which

	def OnLeftDown(self, evt):
		which = self._rget_which2(evt)
		if 0 <= which < 12:
			piece = self.problem.pieces[which]
			if not piece.placed:
				self.moving = piece
				self.moved  = False
				self.xmove  = evt.X
				self.ymove  = evt.Y
				xtop, ytop = self._get_coord2(which/self._columns, which%self._columns, 0, 0)
				self.xoffset = evt.X - xtop
				self.yoffset = evt.Y - ytop
				self.Refresh(False)
			return

		level, index = self._rget_which1(evt)
		if 0 <= level and 0 <= index and level + index < 10:
			self.problem.remove(index, level)
			self.Refresh(False)

	def OnLeftUp(self, evt):
		if self.moving is not None and self.moved:
			level, index = self._rget_which1(evt, self.xoffset, self.yoffset)
			if 0 <= level and 0 <= index and level + index < 10:
				try:
					self.problem.place(self.moving, index, level)
				except Problem.Unplaceable:
					pass
			self.moving = None
			self.moved  = False
			self.Refresh(False)
		else:
			self.moving = None
			which = self._rget_which2(evt)
			if 0 <= which < 12:
				self.problem.pieces[which].rotate()
				self.Refresh(False)

	def OnRightUp(self, evt):
		which = self._rget_which2(evt)
		if 0 <= which < 12:
			self.problem.pieces[which].mirror()
			self.Refresh(False)

	def OnMouseMove(self, evt):
		if self.moving is not None:
			self.moved = True
			self.xmove = evt.X
			self.ymove = evt.Y
			self.Refresh(False)

	def OnLeftDClick(self, evt):
		self.solve()

	def solve(self):
		start = time.time()
		solver = Solver(self.problem)
		for iteration,solution in enumerate(solver.solve(True)):
			if solution:
				self.Refresh(False)
				alert(self, 'Solved; %.2f seconds cost' % (time.time() - start))
				break
		else:
			alert(self, 'No solution; %.2f seconds cost' % (time.time() - start))

	def solve_all(self, fn='pyramid.txt'):
		f = open(fn, 'w')
		def log(msg, *args):
			if msg.startswith('# '):
				msg = '# %s %s' % (time.strftime('%Y-%m-%d %H:%M:%S'), msg[2:])
			if args:
				msg = msg % args
			f.write(msg)
			f.write('\n')
			f.flush()

		solver = Solver(self.problem)
		log('# initial')
		log('%s', self.problem)

		count = 0
		for iteration,solution in enumerate(solver.solve(True)):
			if solution:
				count += 1
				log('# solution #%d', count)
				log('%s', self.problem)
		log('# end, total %d solutions', count)
		f.close()

class MyFrame(wx.Frame):
	def __init__(self):
		title = 'Pyramid Solver'
		size = (535, 785)
		style = wx.MINIMIZE_BOX | wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX | wx.CLIP_CHILDREN
		super(MyFrame, self).__init__(parent=None, title=title, size=size, style=style)

		self._init_menu()

		self.panel = MyPanel(self)

		self.Centre()
		self.Show()

	def _init_menu(self):
		def _add_item(menu, handler, id, text):
			item = menu.Append(id, text)
			self.Bind(wx.EVT_MENU, handler, item)
		def _add_separator(menu):
			menu.AppendSeparator()

		game_menu = wx.Menu()
		_add_item(game_menu, self._on_solve, wx.ID_OPEN, '&Solve\tF12')
		_add_item(game_menu, self._on_solve_all, wx.ID_NEW, '&Solve All')
		_add_separator(game_menu)
		_add_item(game_menu, self._on_quit, wx.ID_EXIT, 'E&xit\tAlt+F4')

		help_menu = wx.Menu()
		_add_item(help_menu, self._on_about, wx.ID_ABOUT, '&About...')

		menu_bar = wx.MenuBar()
		menu_bar.Append(game_menu, '&Game')
		menu_bar.Append(help_menu, '&Help')
		self.SetMenuBar(menu_bar)

	def _on_solve(self, evt):
		self.panel.solve()

	def _on_solve_all(self, evt):
		self.panel.solve_all()

	def _on_quit(self, evt):
		self.Close()

	def _on_about(self, evt):
		info = wx.AboutDialogInfo()
		info.Name = 'Pyramid Solver'
		info.Version = '0.1'
		wx.AboutBox(info)

class Piece(object):
	def __init__(self, color, shape):
		self.color = color
		self._init_shapes(shape)
		self._select_shape(0)
		self.placed = False

	def __str__(self):
		return '\n'.join(''.join(line) for line in self.shape)

	def _init_shapes(self, shape):
		first = map(list, shape)

		first_width = max(map(len, first))
		for line in first:
			while len(line) < first_width:
				line.append(' ')

		mirror = map(list, zip(*first))
		hfirst = [list(reversed(line)) for line in first]
		hmirror = [list(reversed(line)) for line in mirror]

		self.shapes = shapes = [
				first, hmirror, list(reversed(hfirst)), list(reversed(mirror)),
				mirror, hfirst, list(reversed(hmirror)), list(reversed(first)),
				]

		self._variants = variants = []
		for index, shape in enumerate(shapes):
			for another in variants:
				if shape == shapes[another]:
					break
			else:
				variants.append(index)

	def variants(self):
		for index in self._variants:
			self._select_shape(index)
			yield index

	def _select_shape(self, index):
		shapes = self.shapes
		assert 0 <= index < len(shapes)
		self.index = index
		self.shape = shape = shapes[index]
		self.height = len(shape)
		self.width = len(shape[0])

	def rotate(self):
		index = self.index + 1
		if index % 4 == 0:
			index -= 4
		self._select_shape(index)

	def mirror(self):
		self._select_shape((self.index+4) % 8)

class Problem(object):
	class Unplaceable(Exception):
		pass
	class Unremoveable(Exception):
		pass

	raw_pieces = '''
		AAABCCDDDD
		AEEBBCCCD
		AEEFGGGG
		HHIFFFF
		HIIIJJ
		HHIJJ
		KKKJ
		LLK
		LL
		L
		'''

	def __init__(self):
		self.reset()

	def __str__(self):
		return '\n'.join(''.join(line) for line in self.board)

	def reset(self):
		self.board = [[' '] * (10-x) for x in xrange(10)]
		self._init_pieces()

	def place(self, piece, x, y):
		assert not piece.placed

		color = piece.color
		shape, width, height = piece.shape, piece.width, piece.height
		new_board = [line[:] for line in self.board]
		for ox in xrange(width):
			for oy in xrange(height):
				c = shape[oy][ox]
				if c == ' ':
					continue
				x1, y1 = x+ox, y+oy
				if y1 < 0 or y1 >= 10:
					raise Problem.Unplaceable
				if x1 < 0 or x1 >= len(new_board[y1]):
					raise Problem.Unplaceable
				if new_board[y1][x1] != ' ':
					raise Problem.Unplaceable
				new_board[y1][x1] = color

		self.board = new_board
		piece.placed = True

	def remove(self, x, y):
		color = self.board[y][x]
		if color == ' ':
			raise self.Unremoveable
		piece = self.pieces_by_color[color]
		assert piece.placed

		self.board = [[c if c!=color else ' ' for c in line] for line in self.board]
		piece.placed = False

	def remove_piece(self, piece):
		assert piece.placed
		color = piece.color
		self.board = [[c if c!=color else ' ' for c in line] for line in self.board]
		piece.placed = False

	def shuffle(self):
		for piece in self.pieces:
			if piece.placed:
				continue
			rotate = random.randint(0, 3)
			for r in xrange(rotate):
				piece.rotate()
			if random.randint(0, 1) == 0:
				piece.mirror()
		random.shuffle(self.pieces)

	def _init_pieces(self):
		self.pieces = pieces = []
		self.pieces_by_color = pieces_by_color = {}
		colors = sorted(set(self.raw_pieces.strip().replace('\r', '').replace('\n', '').replace('\t', '').replace(' ', '')))
		lines = self.raw_pieces.strip().split()
		for color in colors:
			clines = [''.join([color if c==color else ' ' for c in line]) for line in lines]
			clines = [line for line in clines if line.find(color) >= 0]
			left = min([line.find(color) for line in clines])
			right = max([line.rfind(color) for line in clines])
			clines = [line[left:right+1] for line in clines]
			piece = Piece(color, clines)
			pieces.append(piece)
			pieces_by_color[color] = piece

class Solver(object):
	def __init__(self, problem):
		self.problem = problem

	def solve(self, intermediate=False):
		problem = self.problem
		pieces = problem.pieces
		unplaced = [piece for piece in pieces if not piece.placed]
		if not unplaced:
			return

		def _find_first_empty_cell(problem):
			for level, line in enumerate(problem.board):
				for index, cell in enumerate(line):
					if cell == ' ':
						return level, index
			assert False

		def _place(problem, unplaced, intermediate):
			board = problem.board
			board_height = len(board)
			board_width  = len(board[0])

			level, index = _find_first_empty_cell(problem)
			offset = level + index
			for pi, piece in enumerate(unplaced):
				for variant in piece.variants():
					first = piece.shape[0].index(piece.color)
					if offset - first + piece.height > board_height:
						continue
					if offset - first + piece.width > board_width:
						continue

					try:
						problem.place(piece, index-first, level)
					except Problem.Unplaceable:
						pass
					else:
						if len(unplaced) == 1:
							yield True
						else:
							if intermediate:
								yield False
							if not self._find_hole():
								unplaced2 = unplaced[:]
								unplaced2.pop(pi)
								for solution in _place(problem, unplaced2, intermediate):
									yield solution
						problem.remove_piece(piece)
			return

		for solution in _place(problem, unplaced, intermediate):
			yield solution

	def _find_hole(self, size_threshold=3):
		board = self.problem.board
		holes = []
		for level in xrange(10):
			for index in xrange(10-level):
				if board[level][index] == ' ':
					holes.append([(level, index)])

		holes_length = len(holes)
		while True:
			any_merged = False
			for a in xrange(holes_length):
				acells = holes[a]
				if not acells:
					continue
				for b in xrange(a+1, holes_length):
					bcells = holes[b]
					adjacent = False
					for alevel, aindex in acells:
						for blevel, bindex in bcells:
							if (alevel == blevel and (aindex+1 == bindex or aindex-1 == bindex)) or \
									(aindex == bindex and (alevel+1 == blevel or alevel-1 == blevel)):
								adjacent = True
								break
						if adjacent:
							break
					if adjacent:
						any_merged = True
						acells.extend(bcells)
						bcells[:] = []
			if not any_merged:
				break

		for hole in holes:
			if hole and len(hole) < size_threshold:
				return True
		return False

def test_solver():
	#	raw_pieces = '''
	#		AAABCCDDDD
	#		AEEBBCCCD
	#		AEEFGGGG
	#		HHIFFFF
	#		HIIIJJ
	#		HHIJJ
	#		KKKJ
	#		LLK
	#		LL
	#		L
	#		'''

	problem = Problem()
	pieces = problem.pieces
	problem.place(pieces[0], 0, 0)
	problem.place(pieces[1], 3, 0)
	problem.place(pieces[2], 4, 0)
	problem.place(pieces[3], 6, 0)
	problem.place(pieces[4], 1, 1)
#	problem.place(pieces[5], 3, 2)
#	problem.place(pieces[6], 4, 2)
	for piece in pieces[5:]:
		piece.rotate()
		piece.mirror()
	pieces[5:] = list(reversed(pieces[5:]))

	app = wx.App(redirect=False)
	frame = MyFrame()
	frame.panel.problem = problem
	app.MainLoop()

def main():
	app = wx.App(redirect=False)
	frame = MyFrame()
	app.MainLoop()

if __name__ == '__main__':
#	test_solver()
	main()

