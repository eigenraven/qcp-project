$(function(){
  /* Qubit elements */
  qubits = Array()
  $('.qubit').each((i, el) => {
    qubits.push({
      el: el
    })
  });

  $('#qubit-count').text(qubits.length)

  var ket0
  function get_ket0(){
    if( !ket0 ){
      console.log('Initial |0> MathJax found')
      ket0 = $('.ket0')[0].innerHTML
    }
  }

  var HTML_QUBIT = 


  $('#qubit-more').click(function(){
    get_ket0()
    console.log('more')
    var qnum = qubits.length
    qubits.push({
      el: $(`
      <div class='qubit'>
        <div class='header'>q${String(qnum)}</div>
        <div class='ket0'>${ket0}</div>
      </div>`).appendTo('#qubits')
    })
    $('#qubit-count').text(qubits.length)
  })

  $('#qubit-less').click(function(){
    get_ket0()
    var line
    console.log('fewer')
    if (qubits.length > 0) {
      var line = qubits.pop()
      var lineIsEmpty = true
      if (lineIsEmpty) {
        line.el.remove()
      } else {
        qubits.push(line)
      }
      $('#qubit-count').text(qubits.length)
    }
  })


  /* GATES */
  gates = []

  $.each(gate_kinds, function(id, op){
    op.gate_icon = $('#' + id).click(function(){
      gate = {
        /* TODO: literally everything else */
        el: $(`
        <div class='gate ${id}'>
          ${id}
        </div>
        `).appendTo('#gates')
      }
      gates.push(gate)
      /* TODO: add to qubit log; render nicely */
    })
  })
})
