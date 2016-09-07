#!/usr/bin/python
# -*- encoding: utf-8 -*-

import sys, os, glob
from genshi.template import NewTextTemplate
from api import API

if __name__ == "__main__":
	try:
		tDir = os.path.dirname(__file__)
		for tName in glob.glob(os.path.join(tDir, "*.cpp")) + glob.glob(os.path.join(tDir, "*.hpp")):
			with open(tName) as t, open(os.path.join(os.getcwd(), os.path.basename(tName)), 'w') as f:
				print >> f, NewTextTemplate(t).generate(
					api = API(os.path.expanduser(sys.argv[1]))
				).render()
				
	except IOError as e:
		print e
	except IndexError as e:
		print >> sys.stderr, "Usage: %s <swagger.yaml>" % sys.argv[0]
