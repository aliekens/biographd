#!/usr/bin/ruby

#   Biograph computing kernel daemon
#   Copyright (C) 2013-2016  Anthony Liekens

#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.

#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

# run both the biograph deamon and this client.rb from within biographlibrary/example/biograph

require 'biographdclient'
require 'pp'

@@conceptid2umlsidindex = {}
@@umlsid2conceptidindex = {}
def loadconceptindices
  $stderr.print "Loading concepts.tsv"
  File.open( 'concepts.tsv' ).each do | line |
    fields = line.strip.split
    @@conceptid2umlsidindex[ fields[ 0 ].to_i ] = fields[ 1 ]
    @@umlsid2conceptidindex[ fields[ 1 ] ] = fields[ 0 ].to_i
  end
  $stderr.puts " (#{@@conceptid2umlsidindex.size})"
end
loadconceptindices

def umlsid2conceptid( umlsid )
  if @@umlsid2conceptidindex.size == 0
    loadconceptindices
  end
  return @@umlsid2conceptidindex[ umlsid ]
end

def conceptid2umlsid( conceptid )
  if @@conceptid2umlsidindex.size == 0
    loadconceptindices
  end
  return @@conceptid2umlsidindex[ conceptid ]
end

client = BioGraphDClient.new( "http://localhost:9018/RPC2" )

# define a set of sources

sources = { umlsid2conceptid( "C0036341" ) => 0.5, umlsid2conceptid( "C0005586" ) => 0.5 }

# find concepts related to this set
counter = 1
puts "Related to both schizophrenia & bipolar"
results = client.getRelatedEntities( sources, { :number => 5, :inferred => 0, :type => 0, :cache => "cache/sets/schizophreniabipolar.bin", :timeout => -1, :target_entityset => 46 } )
results.sort{|x,y|y[1]<=>x[1]}.each do |concept, score|
  puts "#{counter}\t#{score}\t#{conceptid2umlsid(concept)}"
  counter += 1
end
puts

# define a target

target = umlsid2conceptid( "C1427080" )

# position of target

puts "Position of TPH2 and dopamine from both schizophrenia & bipolar"
pp client.getPositions( sources, [ target, umlsid2conceptid( "C0013030" ) ], { :cache => "cache/sets/schizophreniabipolar.bin", :timeout => -1 } )
puts

# find paths from the set to this target
counter = 1
puts "Path to TPH2 from both schizophrenia & bipolar"
results = client.getPaths( sources, target, { :number => 10, :cache => "cache/sets/schizophreniabipolar.bin", :timeout => -1 } )
results.sort{|x,y|y[0]<=>x[0]}.each do |score, path|
  print "#{counter}\t#{score}\t"
  path.each do |node| 
    print "#{conceptid2umlsid(node)}\t"
  end
  puts
  counter += 1
end
puts

# compare collections
puts "Comparing two collections"
results = client.getComparison( { :collection1id => 3, :collection2id => 4, :filtertype => 5 } )

puts "Context of a concept in this comparison"
results = client.getComparisonContext( { :collection1id => 3, :collection2id => 4, :concept => 22287, :number => 10 } )
results = client.getComparisonContext( { :collection1id => 3, :collection2id => 4, :concept => 29260, :number => 20 } )
results = client.getComparisonContext( { :collection1id => 3, :collection2id => 4, :concept => 59400, :number => 30 } )
