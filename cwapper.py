#!/usr/bin/python
# -*- encoding: utf-8 -*-

import sys, os
from genshi.template import NewTextTemplate
from api import API

if __name__ == "__main__":
	try:
		with open(os.path.join(os.path.dirname(__file__), 'template.cpp')) as t:
			print NewTextTemplate(t).generate(
				api = API(os.path.expanduser(sys.argv[1]))
			).render()
	except IOError as e:
		print e
	except IndexError as e:
		print >> sys.stderr, "Usage: %s <swagger.yaml>" % sys.argv[0]
