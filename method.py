# -*- encoding: utf-8 -*-
import sys
class Method:
	from parameter import Parameter
	
	def __init__(self, path, name, yaml={}):
		self.path = path
		self.name = name.upper()
		self.yaml = yaml
		self.parameters = tuple(
			self.Parameter(self, parameter['name'], parameter)
				for parameter in self.yaml.get('parameters', {})
		)
		if 'application/json' not in self.produces:
			print >> sys.stderr, "WARNING: %s %s does not produce ONLY supported 'applcation/json'" % (self.name, self.path.uri)
	
	def __repr__(self):
		return "%s: %s" % (self.__class__.__name__, self.name)

	def __getattr__(self, name):
		return self.yaml.get(name)
	
	@property
	def signature(self):
		return ', '.join(p.signature for p in self.parameters)
	
	@property
	def call(self):
		return ', '.join(p.surname for p in self.parameters)
