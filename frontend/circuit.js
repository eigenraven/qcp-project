$(function(){
  const GATE_ALIAS = {
    "nop": "id", "hadamard": "h", "toffoli": "ccnot"
  }

  /* Qubit elements */
  qubits = Array()
  $('.qubit').each((i, el) => {
    qubits.push({
      el: el
    })
  });

  $('.disp-qubits').text(qubits.length)

  let ket0;
  function get_ket0(){
    if( !ket0 ){
      console.log('Initial |0> MathJax found')
      ket0 = $('.ket0')[0].innerHTML
    }
  }

  $('#btn-qubit-more').click(function(){
    get_ket0()
    console.log('more')
    let qnum = qubits.length
    qubits.push({
      el: $(`
      <div class='qubit'>
        <div class='header'>q${String(qnum)}</div>
        <div class='ket0'>${ket0}</div>
      </div>`).appendTo('#qubits')
    })
    $('.disp-qubits').text(qubits.length)
  })

  $('#btn-qubit-less').click(function(){
    get_ket0()
    let line
    console.log('fewer')
    if (qubits.length > 0) {
      let line = qubits.pop()
      let lineIsEmpty = true
      if (lineIsEmpty) {
        line.el.remove()
      } else {
        qubits.push(line)
      }
      $('.disp-qubits').text(qubits.length)
    }
  })

  /* PANEL */
  let shots = 1024;

  let updateSlider = function(){
    shots = 2 ** $('#slider-shots').val()
    $('.disp-shots').text(shots)
  }; updateSlider()
  $('#slider-shots').change(updateSlider)
  $('#slider-shots').mousemove(updateSlider)

  let noise = 0;

  let updateNoise = function(){
    shots = $('#slider-noise').val() / 10000
    let s = (shots * 100).toLocaleString({maximumFractionDigits: 2})
    $('.disp-noise-full').text(
      shots ? `${s}%`: "disabled")
    $('.disp-noise-short').text(
      shots ? `, ${s}% decay` : ``)
  }; updateNoise()
  $('#slider-noise').change(updateNoise)
  $('#slider-noise').mousemove(updateNoise)

  let isSparse = false;
  $('#check-sparse').change(function(){
    isSparse = $('#check-sparse').is(':checked')
  });

  let doGroup = true;
  $('#check-group').change(function(){
    doGroup = $('#check-group').is(':checked')
  });

  /* GATES */
  gates = []

  $.each(GATES, function(id, op){
    op.gate_icon = $('#' + id).click(function(){
      gate = {
        id: id,
        qubits: [], // add qubits..?
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

  /* FILE IO */

  as_file = function(){
    // TODO: implement form & gathering
    emitStates = true;

    str = `qubits,${qubits.length}\n`
    if(shots != 1024) str += `shots,${shots}\n`
    if(noise) str += `noise,${noise}\n`
    if(isSparse) str += `sparse\n`
    if(!doGroup) str += `nogroup\n`
    if(emitStates) str += `states\n`
    
    $.each(gates, function(i, gate){
      console.log(gate)
      str += `${gate.id}`
      $.each(gate.qubits, (i,q) => str+= `,${q}`)
      str += `\n`
    })
    return str
  }
  function download(data, filename, type) {
    let file = new Blob([data], {type: type});
    if (window.navigator.msSaveOrOpenBlob) // IE10+
        window.navigator.msSaveOrOpenBlob(file, filename);
    else { // Others
      let a = document.createElement("a"),
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

  
  $('#btn-load').click(function(){
    $('#btn-load')[0].files[0].text()
    .then(function(str){
      let hasError = false
      let f_gates = [];
      let f_qubits;
      let f_noise = 0;
      let f_shots = 1024;
      let f_isSparse = false;
      let f_doGroup = true;
      let f_emitStates = true;

      function error(lineno, msg){
        msg = `Error in line ${lineno}: ${msg}`
        $('#load-error').text(msg)
        console.log(msg)
        hasError = true
      }

      $.each(str.split(/\r\n|\r|\n/), function(l, line){
        if( !hasError ){
          if( !line || lines.startsWith(`//`)){
            return; // strip comments
          }
          let args = line.split(",").toLowerCase()
          let name = args.shift()
          
          function numHeader(val, name, cond){
            let token = args.shift()
            if( token == undefined ){
              error(l, `${name} requires a value`)
            }
            let numval = Number(token)
            if( cond(numval) ){
              console.log(name, "obtained", numval)
              return numval
            } else {
              error(l, `${name} cannot be ${token}`)
              return val
            }
          }
          
          switch( name ){
            case "qubits": f_qubits = numHeader(
              f_qubits, "Qubit count",
              n => Number.isInteger(n) && n > 0
            ); break;

            case "shots": f_shots = numHeader(
              f_shots, "Shots",
              n => Number.isInteger(n) && n > 0
            );
            case "noise": f_noise = numHeader(
              f_noise, "Noise",
              n => 0 <= n && n <= 1
            )
            
            case "sparse": f_isSparse = true; break;
            case "nogroup": f_doGroup = false; break;
            case "states": f_emitStates = true; break;

            default:
              let gate = GATES[name];
              if( !gate ){
                error(l, `Unknown operation "${name}"`)
              }
              if( args.length <= gate.args.length ){
                error(l, `Not enough args to ${name} (need ${arity})`)
              } else {
                // TODO: assert stuff with indices here
                f_gates.push({
                  id: name
                })
              }
              break;
            }
          }
        })
        if( !hasError ){
          console.log('loaded successfully!')
          console.log(f_qubits, f_shots, f_noise)
          console.log(f_doGroup, f_emitStates, f_isSparse)
          console.log(f_gates)
          // TODO: actually load
        } else {
          console.log('loaded abysmally!')
        }
      })
      console.log(str)
    })
})
