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
    $('#btn-qubit-count').text(qubits.length)
  })

  $('#btn-qubit-less').click(function(){
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
        id: id,
        qubits: [],
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
  
  as_file = function(){
    // TODO: implement form & gathering
    isSparse = false
    doGroup = true
    noise = 0.0
    shots = 1024

    str = `qubits,${qubits.length}\n`
    if(shots != 1024){str += `shots,${shots}\n`}
    if(noise){str += `noise,${noise}\n`}
    if(isSparse){str += `sparse\n`}
    if(!doGroup){str += `nogroup\n`}
    /* `states` ..? */
    
    $.each(gates, function(i, gate){
      console.log(gate)
      str += `${gate.id}`
      $.each(gate.qubits, (i,q) => str+= `,${q}`)
      str += `\n`
    })
    return str
  }

  
function download(data, filename, type) {
  var file = new Blob([data], {type: type});
  if (window.navigator.msSaveOrOpenBlob) // IE10+
      window.navigator.msSaveOrOpenBlob(file, filename);
  else { // Others
      var a = document.createElement("a"),
              url = URL.createObjectURL(file);
      a.href = url;
      a.download = filename;
      document.body.appendChild(a);
      a.click();
      setTimeout(function() {
          document.body.removeChild(a);
          window.URL.revokeObjectURL(url);  
      }, 0); 
    }
  }
  $('#btn-save').click(function(){
    download(as_file(), "circuit.in", "text/plaintext")
  })
})
