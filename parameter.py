# -*- encoding: utf-8 -*-

class Parameter:

	def __init__(self, method, name, yaml={}):
		self.method = method
		self.name = name
		self.yaml = yaml
	
	iN = lambda self, where: self.yaml['in'] == where

	def __repr__(self):
		return "%s: %s<%s> in %s" % (self.__class__.__name__, self.name, self.yaml.get('type'), self.yaml['in'])

	def __getattr__(self, name):
		return self.yaml.get(name)
	
	def __nonzero__	(self):
		return True
	
	@property
	def definition(self):
		if self.iN('body') and '$ref' in self.yaml['schema']:
			ref = self.yaml['schema']['$ref']
			return self.method.path.api.definitions[ref]
			
		return None

	@property
	def surname(self):
		if self.iN('path') and self.ctype != 'std::string':
			return "_%s" % self.name
			
		return self.name
		
	@property
	def declaration(self):
		return '%s %s' % (self.ctype, self.surname)
	@property
	def signature(self):
		return '%s %s' % (self.ctype, self.name)

	@property
	def retrieve(self):
		if self.iN('path') and self.ctype != 'std::string':
			return '%s = atoi(%s.c_str());' % (self.declaration, self.name);

		if self.iN('query'):
			return '%s = boost::get<%s>(query["%s"]);' % (self.declaration, self.ctype, self.name)

		if self.iN('formData') or self.iN('header'):
			return '%s = request().getenv("HTTP_%s");' % (self.declaration, self.name.upper());

		if self.iN('body'):
			if self.isArray:
				return '%s = this->body().array();' % self.declaration
			else:
				return '%s = this->body();' % self.declaration

	@property
	def isArray(self):
		return "array" in self.ctype or "vector" in self.ctype

	@property
	def type(self):
		return self.yaml.get('type')
		
	@property
	def ctype(self):
		if self.type:
			return { 
				'array': 'cwapper::stringarray',
				'integer': 'int',
				'string': 'std::string',
			}.get(self.type)

		if '$ref' in self.yaml['schema']:
			return 'cppcms::json::value'

		if 'items' in self.yaml['schema']:
			return 'cppcms::json::array'
		
