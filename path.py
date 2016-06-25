# -*- encoding: utf-8 -*

class Path:
	from method import Method
	
	def __init__(self, api, uri, yaml={}):
		self.api = api
		self.uri = uri
		self.yaml = yaml
		self.methods = tuple(
			self.Method(self, name, yaml) 
				for name, yaml in yaml.iteritems()
		)
		self.parameters = tuple(
			p
				for m in self.methods
					for p in m.parameters
		)

	def __repr__(self):
		return "%s: %s" % (self.__class__.__name__, self.name)

	def __getattr__(self, name):
		return self.yaml.get(name)
