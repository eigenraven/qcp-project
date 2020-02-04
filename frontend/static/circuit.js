console.log('what the')
var circuits = Array()

$(function(){

  $('#qubit-more').click(function(){
    console.log('more')
    circuits.push({
    
    })
    $('#qubit-count').text(circuits.length)
  })
  .click().click()

  $('#qubit-less').click(function(){
    console.log('fewer')
    var line = circuits.pop()
    // TODO: Check if line is in use!
    if (!line.inUse) {
      circuits.push(line)
    }
    $('#qubit-count').text(circuits.length)
  })s
})
