$(function(){
  var circuits = Array()
  $('.qubit').each(el => {
    circuits.push({
      el: el
    })
  });
  $('#qubit-count').text(circuits.length)
  
  var HTML_QUBIT = `
    <div class='qubit'>
      <div class='info'>
        %N
      </div>
      <div class='dirac'>
        %KET0
      </div>
    </div>`
  $('#qubit-more').click(function(){
    console.log('more')
    var qnum = circuits.length
    circuits.push({
      el: $(
        HTML_QUBIT
        .replace('%N', String(qnum))
        .replace('%KET0', $('.qubit .dirac')[0].innerHTML)
      ).appendTo('#qubits')
    })
    $('#qubit-count').text(circuits.length)
  })

  $('#qubit-less').click(function(){
    var line
    console.log('fewer')
    if (circuits.length > 0) {
      var line = circuits.pop()
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
