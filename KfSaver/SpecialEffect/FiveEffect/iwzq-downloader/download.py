
import httplib, re, socket, string

proxy = "cn-pxy08"
proxy_port = 8080
download_retry = 4


class DownloadError(Exception):
	pass

class ParseError(Exception):
	pass

def download_no_retry(url, proxy=None, proxy_port=80):
	try:
		if proxy is not None:
			conn = httplib.HTTPConnection("%s:%d" % (proxy, proxy_port))
		else:
			host = re.compile(r"^http://([A-Za-z0-9_\.\-]+)/").match(url).group(1)
			conn = httplib.HTTPConnection(host)
		conn.request("GET", url)
		response = conn.getresponse()
		if response.status != 200:
			raise DownloadError, "Download failed: %d %s" % (response.status, response.reason)
		data = response.read()
		if proxy is not None and data.lower().find(proxy.lower()) >= 0:
			raise DownloadError, "\"%s\" found in the download data, download failed..." % proxy
		conn.close()
		return data
	except httplib.HTTPException:
		raise DownloadError, "httplib.HTTPException"
	except socket.error:
		raise DownloadError, "socket.error"

def download(url, retry=4, proxy=None, proxy_port=80):
	for count in range(1, retry):
		try:
			return download_no_retry(url, proxy, proxy_port)
		except DownloadError:
			pass
	return download_no_retry(url, proxy, proxy_port)

def parse(data):
	ptn = re.compile(r"""<iframe src="makemove\.asp\?act=view&moves=([a-oA-O0-9]+)&steps=(\d+)\" """)
	m = ptn.search(data)
	if m is None:
		raise ParseError, "ParseError"

	moves = m.group(1)
	steps = m.group(2)

	try:
		steps = int(steps)
	except:
		raise ParseError, "Failed to convert %s to an integer" % steps

	moves = filter(None, re.split("(\d+)", moves))
	if len(moves) != 2 * steps:
		raise ParseError, "steps error"
	moves = map(lambda x: ord(x.upper())-ord('A')+1 if x in string.ascii_letters else 16 - int(x), moves)
	moves = zip(moves[1::2], moves[0::2])
	return moves

def gen_game(moves):
	left = min([x[1] for x in moves])
	right = max([x[1] for x in moves])
	top = min([x[0] for x in moves])
	bottom = max([x[0] for x in moves])

	board = [[' ']*15 for i in range(15)]
	i = 1
	for move in moves:
		board[move[0]-1][move[1]-1] = str(i)
		i += 1

	board = ["\t".join(line[left-1:]).rstrip().replace(" ", "") for line in board]
	board = [chr(ord('A')+i) + "\t\t" + board[i] for i in range(15)]
	board = board[top-1:bottom]
	column = "COL\t\t" + "\t".join([str(i) for i in range(left, right+1)])
	return column + "\n\n" + "\n".join(board)

def urls():
	for qpid in range(93, 1811):
		yield r"http://www.iwzq.com/qplist.asp?qpid=%d&menu=4&returnmenu=2" % qpid

def all(file):
	for url in urls():
		try:
			print "Try %s ..." % url
			data = download(url, 4, "cn-pxy08", 8080)
			moves = parse(data)
			game = gen_game(moves)
			file.write("""#
#############################################################################
#	%s
#
%s
""" % (url, game))
			file.flush()
		except:
			print "Failed to process %s, skipped" % url

if __name__ == "__main__":
	all(open("iwzq.txt", "w"))

