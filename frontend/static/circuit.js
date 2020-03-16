$(function(){
  /*  */
  gates = Array()

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

  var HTML_QUBIT = `
    <div class='qubit'>
      <p class='info'>
        qubit %N
      </p>
      <div class='ket0'>
        %KET0
      </div>
    </div>`


  $('#qubit-more').click(function(){
    get_ket0()
    console.log('more')
    var qnum = qubits.length
    qubits.push({
      el: $(
        HTML_QUBIT
        .replace('%N', String(qnum))
        .replace('%KET0', ket0)
      ).appendTo('#qubits')
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
})
