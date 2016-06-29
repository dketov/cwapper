# CwaPPer
cppcms restful api server code generator from swagger.yaml

0. Install dependencies 
   
   * Install cmake, make, g++
   * Install genshi (https://genshi.edgewall.org/), pyyaml (http://pyyaml.org/)

1. Install cppcms
   
   `svn co https://svn.code.sf.net/p/cppcms/code/framework/tags/v1.0.5`
   
   see http://cppcms.com/wikipp/en/page/cppcms_1x_build for installation instructions

2. Clone this repo and compile the test server

   ```sh
   git clone https://github.com/dketov/cwapper.git && cd cwapper
   mkdir __build__ && cd __build__
   cmake ../tests/petshop
   make
   ```
  
3. Run the test server
   ```sh
   ./api.sh
   ```
  
4. Try swagger-editor against running server
   * Open http://editor.swagger.io/#/ in browser
   * File/Import file ../tests/petshop/api.yaml
   Or try built-in swagger-ui
   * Open http://localhost:8080 in browser
   
   Explore!
