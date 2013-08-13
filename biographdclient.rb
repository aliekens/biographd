require 'xmlrpc/client'

require 'pp'

SIZEOFDOUBLE = [1.to_f].pack("d").size
SIZEOFINTEGER = [1.to_i].pack("i").size

class BioGraphDClient

  def initialize( url )
    @server = XMLRPC::Client.new2( url )
    @server.timeout = 300
  end
  
  # get top hub concepts in reference to a list of source concepts
  # returns probabilities and concept IDs
  def getPositions( sources, targets, parameters )
    # XMLRPC request
    response = @server.call( 
      "biograph.position", 
      sources.keys,
      sources.values,
      targets,
      Struct.new(*parameters.keys).new(*parameters.values) # this needs ruby > 1.9
      )
    result = {}
    offset = 0
    targets.each do | target |
      responsepart = response.unpack( "@#{offset}idiiii" )
      result[ responsepart[ 0 ] ] = { 
        :score => responsepart[ 1 ],
        :position => responsepart[ 2 ],
        :total => responsepart[ 3 ],
        :positionByType => responsepart[ 4 ],
        :totalByType => responsepart[ 5 ]
      }
      offset += 5 * SIZEOFINTEGER + SIZEOFDOUBLE
    end
    return result
  end

  # get top hub concepts in reference to a list of source concepts
  # returns probabilities and concept IDs
  def getRelatedEntities( sources, parameters )
    # XMLRPC request
    response = @server.call( 
      "biograph.relatedentities", 
      sources.keys,
      sources.values,
      Struct.new(*parameters.keys).new(*parameters.values) # this needs ruby > 1.9
      )
    
    # format in usable format
    offset = 0
    result = []
    ( 0 .. parameters[ :number ] - 1 ).each do |i|
      # unpack binary stream
      responsepart = response.unpack( "@#{ offset }id" )
      if responsepart[ 0 ] != 0
        offset += SIZEOFINTEGER + SIZEOFDOUBLE
        result << [ responsepart[ 0 ], responsepart[ 1 ], responsepart[ 2 ].to_i ]
      end
    end
    return result
  end

  # get top hub concepts in reference to a list of source concepts
  # returns probabilities and concept IDs
  def getPaths( sources, target, parameters )
    # XMLRPC request
    response = @server.call( 
      "biograph.paths", 
      sources.keys,
      sources.values,
      target,
      Struct.new(*parameters.keys).new(*parameters.values) # this needs ruby > 1.9
      )
    
    # format in usable format
    paths = []
    offset = 0
    ( 1 .. 10 ).each do |pathcounter|
      data = response.unpack( "@#{ offset }di" ) # first = probability of the path, second = number of nodes in the path
      offset += SIZEOFDOUBLE + SIZEOFINTEGER
      probability = data[ 0 ]
      path = response.unpack( "@#{ offset }#{ "i" * data[ 1 ] }" ) # path is encoded as a string of integer IDs
      offset += data[ 1 ] * SIZEOFINTEGER
      paths << [ probability, path ]
    end
    
    return paths;
    
  end
  
  # compare two collections
  
  def getComparison( parameters )

    # XMLRPC request
    response = @server.call( 
      "biograph.comparison", 
      Struct.new(*parameters.keys).new(*parameters.values) # this needs ruby > 1.9
      )
    
    # unpack data for "size" datapoints
    result = []
    offset = 0;
    size = response.unpack( "@#{ offset }i" )[0]
    offset += SIZEOFINTEGER
    ( 1 .. size ).each do | counter |
      data = response.unpack( "@#{ offset }idd" ) # id, logratio, minuslogpvalue
      offset += SIZEOFINTEGER + 2 * SIZEOFDOUBLE
      result << data
    end
    
    return result
    
  end

  def getComparisonContext( parameters )

    # XMLRPC request
    response = @server.call( 
      "biograph.comparisoncontext", 
      Struct.new(*parameters.keys).new(*parameters.values) # this needs ruby > 1.9
      )
    
    # unpack data for relevant neighbors
    relevantneighbors = []
    offset = 0;
    size = response.unpack( "@#{ offset }i" )[0]
    offset += SIZEOFINTEGER
    ( 1 .. size ).each do | counter |
      data = response.unpack( "@#{ offset }idd" ) # id, logratio, minuslogpvalue
      offset += SIZEOFINTEGER + 2 * SIZEOFDOUBLE
      relevantneighbors << data
    end
    
    # unpack values of the concept for collection1
    collection1 = []
    size = response.unpack( "@#{ offset }i" )[0]
    offset += SIZEOFINTEGER
    ( 1 .. size ).each do | counter |
      data = response.unpack( "@#{ offset }d" )[0] # iweight
      offset += SIZEOFDOUBLE
      collection1 << data
    end

    # unpack values of the concept for collection1
    collection2 = []
    size = response.unpack( "@#{ offset }i" )[0]
    offset += SIZEOFINTEGER
    ( 1 .. size ).each do | counter |
      data = response.unpack( "@#{ offset }d" )[0] # iweight
      offset += SIZEOFDOUBLE
      collection2 << data
    end

    return [ relevantneighbors, collection1, collection2 ]
    
  end

end
