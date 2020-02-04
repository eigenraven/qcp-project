console.log('what the')
var circuits = Array()

$(function(){
  $('#qubit-more').click(function(){
    console.log('more')
    circuits.push({
      el: $('<div/>', {
        "class": "qubit"
      }).appendTo('#qubits')
    })
    $('#qubit-count').text(circuits.length)
  })
  .click().click()

  $('#qubit-less').click(function(){
    var line
    console.log('fewer')
    if (line = circuits.pop()) {
      var lineIsEmpty = true
      if (lineIsEmpty) {
        line.el.remove()
      } else {
        circuits.push(line)
      }
      $('#qubit-count').text(circuits.length)
    }
  })
})
