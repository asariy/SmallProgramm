import sys

LEAR = {
	'S': 1, 
	'D': 2,
	'H': 3,
	'C': 4 
}

NUM = {
	'2': 2,
	'3': 3,
	'4': 4,
	'5': 5,
	'6': 6,
	'7': 7,
	'8': 8,
	'9': 9,
	'T': 10,
	'J': 11,
	'Q': 12,
	'K': 13,
	'A': 14
}

def get_lear(str):
	return LEAR[str]

def get_num(str):
	return NUM[str]

def flash_royal(cards):
	if cards[0].lear == cards[1].lear == cards[2].lear == cards[3].lear == cards[4].lear \
		and cards[0].num + 4 == cards[1].num + 3 == cards[2].num + 2 == cards[3].num + 1 == cards[4].num == 14:
		return [10]

def street_flash(cards):
	if cards[0].lear == cards[1].lear == cards[2].lear == cards[3].lear == cards[4].lear \
		and cards[0].num + 4 == cards[1].num + 3 == cards[2].num + 2 == cards[3].num + 1 == cards[4].num:
		return [9, cards[4].num]
	if cards[0].lear == cards[1].lear == cards[2].lear == cards[3].lear == cards[4].lear \
		and cards[0].num + 3 == cards[1].num + 2 == cards[2].num + 1 == cards[3].num == cards[4].num - 9:
		return [9, cards[3].num]

def care(cards):
	if cards[0].num == cards[1].num == cards[2].num == cards[3].num:
		return [8, cards[0].num, cards[4].num]
	if cards[1].num == cards[2].num == cards[3].num == cards[4].num:
		return [8, cards[4].num, cards[0].num]
	
def full_house(cards):
	if cards[0].num == cards[1].num == cards[2].num and cards[3].num == cards[4].num:
		return [7, cards[0].num, cards[4].num]
	if cards[0].num == cards[1].num and cards[2].num == cards[3].num == cards[4].num:
		return [7, cards[4].num, cards[0].num]

def flash(cards):
	if cards[0].lear == cards[1].lear == cards[2].lear == cards[3].lear == cards[4].lear:
		return [6, cards[4].num, cards[3].num, cards[2].num, cards[1].num, cards[0].num]

def street(cards):
	if cards[0].num + 4 == cards[1].num + 3 == cards[2].num + 2 == cards[3].num + 1 == cards[4].num:
		return [5, cards[4].num]
	if cards[0].num + 3 == cards[1].num + 2 == cards[2].num + 1 == cards[3].num == cards[4].num - 9:
		return [5, cards[3].num]


def triple(cards):
	if cards[0].num == cards[1].num == cards[2].num:
		return [4, cards[0].num, cards[4].num, cards[3].num]
	if cards[1].num == cards[2].num == cards[3].num:
		return [4, cards[1].num, cards[4].num, cards[0].num]
	if cards[2].num == cards[3].num == cards[4].num:
		return [4, cards[2].num, cards[1].num, cards[0].num]

def two_double(cards):
	if cards[0].num == cards[1].num and cards[2].num == cards[3].num:
		return [3, max(cards[0].num, cards[2].num), min(cards[0].num, cards[2].num), cards[4].num]
	if cards[0].num == cards[1].num and cards[3].num == cards[4].num:
		return [3, max(cards[0].num, cards[3].num), min(cards[0].num, cards[3].num), cards[2].num]
	if cards[1].num == cards[2].num and cards[3].num == cards[4].num:
		return [3, max(cards[1].num, cards[3].num), min(cards[1].num, cards[3].num), cards[0].num]

def double(cards):
	if cards[0].num == cards[1].num:
		return [2, cards[0].num, cards[4].num, cards[3].num, cards[2].num]
	if cards[1].num == cards[2].num:
		return [2, cards[1].num, cards[4].num, cards[3].num, cards[0].num]
	if cards[2].num == cards[3].num:
		return [2, cards[2].num, cards[4].num, cards[1].num, cards[0].num]
	if cards[3].num == cards[4].num:
		return [2, cards[3].num, cards[2].num, cards[1].num, cards[0].num]

def single(cards):
	return [1, cards[4].num, cards[3].num, cards[2].num, cards[1].num, cards[0].num]

class Card(object):
	def __init__(self, str):
		super(Card, self).__init__()
		self.num = get_num(str[0])
		self.lear = get_lear(str[1])

class Hand(object):
	def __init__(self, cards):
		super(Hand, self).__init__()
		self.cards = cards
		self.cards = sorted(self.cards, key=lambda card: card.num)
		self.raiting = flash_royal(self.cards)
		if not self.raiting: self.raiting = street_flash(self.cards)
		if not self.raiting: self.raiting = care(self.cards)
		if not self.raiting: self.raiting = full_house(self.cards)
		if not self.raiting: self.raiting = flash(self.cards)
		if not self.raiting: self.raiting = street(self.cards)
		if not self.raiting: self.raiting = triple(self.cards)
		if not self.raiting: self.raiting = two_double(self.cards)
		if not self.raiting: self.raiting = double(self.cards)
		if not self.raiting: self.raiting = single(self.cards)

		


in_data = sys.argv[1]
in_file = open(in_data, 'r')

out_data = sys.argv[2]
out_file = open(out_data, 'w')

for line in in_file:
	cards = []
	line = line.upper()
	for card in line.split(' '):
		cards.append(Card(card))

	player_one = Hand(cards[0:5])
	player_two = Hand(cards[5:10])

	it = 0
	winer = None

	while not winer:
		if player_one.raiting[it] != player_two.raiting[it]:
			if player_one.raiting[it] > player_two.raiting[it]:
				winer = 1
			else:
				winer = 2
		else:
			it += 1

	out_file.write(str(winer) + '\n')
