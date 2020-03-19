$(function(){
  const GATE_ALIAS = {
    "nop": "id", "hadamard": "h", "toffoli": "ccnot"
  }

  /* PANEL */
  qubits = Array()

  let conf_default = function(){return {
    noise: 0,
    shots: 1024,
    isSparse: false,
    doGroup: true,
    emitStates: true,
  }}

  let ket0;
  let conf = conf_default();
  let loadingError = false;

  const NOISE_SIZE = 10000

  let updateDisplay = function(){
    conf.shots = 2 ** $('#slider-shots').val()
    conf.noise = $('#slider-noise').val() / NOISE_SIZE
    conf.isSparse = $('#check-sparse').is(':checked')
    conf.doGroup = $('#check-group').is(':checked')

    if( !ket0 ){
      ket0 = $('.ket0')[0].innerHTML
      console.log('Initial |0> MathJax found:', ket0)
    }

    $('.disp-qubits').text(qubits.length)
    $('.disp-shots').text(conf.shots)
    let fragments = [
      `qubits: ${qubits.length}`,
      `shots: ${conf.shots}`
    ]
    let n = (conf.noise * 100).toLocaleString({maximumFractionDigits: 2})
    $('.disp-noise').text(
      conf.noise ? `${n}%`: "disabled")
    
    conf.noise ? fragments.push(`${n}% decay`) :0;
    conf.isSparse ? fragments.push("sparse") :0;
    conf.doGroup ? fragments.push("grouped") :0;

    $('.disp-summary').text(`(${fragments.join(", ")})`)
  }
  updateDisplay()

  let addQubit = function(){
    console.log(`qubits +> ${qubits.length + 1}`)
    qubits.push({
      el: $(`
      <tr class='qubit'>
        <div class='line'></div>
        <td class='header'>q[${qubits.length}]</td>
        <td>${ket0}</td>
        <td class='fill'></td>
      </tr>`).appendTo('#qubits')
    })
  };
  addQubit(); addQubit();
  $('#btn-qubit-more').click(function(){
    addQubit()
    updateDisplay()
  });

  $('#btn-qubit-less').click(function(){
    if (qubits.length > 0) {
      console.log(`qubits -> ${qubits.length - 1}`)
      let line = qubits.pop()
      let lineIsEmpty = true
      if (lineIsEmpty) {
        line.el.remove()
      } else {
        qubits.push(line)
      }
      updateDisplay()
    }
  })

  $('#slider-shots').change(updateDisplay)
  $('#slider-shots').mousemove(updateDisplay)
  $('#slider-noise').change(updateDisplay)
  $('#slider-noise').mousemove(updateDisplay)

  $('#check-sparse').change(updateDisplay)
  $('#check-group').change(updateDisplay)

  /* GATES */
  let selection = {
    gate: null,
    arg: 0
  }

  gates = []

  $.each(GATES, function(id, op){
    op.el = $('#' + id).click(function(){
      console.log(id, GATES[id])
      if( selection.gate && selection.gate !== id ){
        GATES[selection.gate].el.removeClass('selected')
      }
      if( selection.gate == id ){
        selection.gate = null
        GATES[id].el.removeClass('selected')
      } else {
        selection.gate = id
        selection.arg = 0
        GATES[id].el.addClass('selected')
      }
      let c = $('#circuit')
      (selection.gate ? c.addClass : c.removeClass)('selection')
    })
  })



  /* FILE IO */



  as_file = function(){
    // TODO: implement form & gathering
    emitStates = true;

    str = `qubits,${qubits.length}\n`
    if(conf.shots != 1024) str += `shots,${conf.shots}\n`
    if(conf.noise) str += `noise,${conf.noise}\n`
    if(conf.isSparse) str += `sparse\n`
    if(!conf.doGroup) str += `nogroup\n`
    if(conf.emitStates) str += `states\n`
    
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
  
  let filename = "circuit.in"
  $('#btn-save').click(function(){
    download(as_file(), filename, "text/plaintext")
  })

  
  $('#btn-load').change(function(){
    let file = $('#btn-load')[0].files[0]
    filename = file.name;
    loadingError = false

    console.log(`Loading file ${filename}...`)
    $('.disp-load-name').removeClass('error')
    $('.disp-load-error').text("")

    function error(lineno, msg){
      msg = `Error in line ${lineno}: ${msg}`
      $('.disp-load-error').text(msg)
      $('.disp-load-name').addClass('error').text("file failed to load!")
      console.log(msg)
      loadingError = true
    }
    
    let f_gates = [];
    let f_conf = conf_default();

    file.text().then(function(str){
      $.each(str.split(/\r\n|\r|\n/), function(l, line){
        if( !loadingError ){
          if( !line || line.startsWith(`//`)){
            return; // strip comments
          }
          let args = line.split(",")
          let name = args.shift().toLowerCase()
          
          function numHeader(cond){
            let token = args.shift()
            if( token == undefined ){
              error(l, `'{desc}' requires a value`)
            }
            let numval = Number(token)
            if( cond(numval) ){
              console.log(name, "obtained", numval)
              f_conf[name] = numval
            } else {
              error(l, `'${name}' cannot be ${token}`)
            }
          }
          
          switch( name ){
            case "qubits": numHeader(
              n => Number.isInteger(n) && n > 0
            ); break;
            case "shots": numHeader(
              n => Number.isInteger(n) && n > 0
            ); break;
            case "noise": numHeader(
              n => 0 <= n && n <= 1
            ); break;
            case "sparse": f_conf.isSparse = true; break;
            case "nogroup": f_conf.doGroup = false; break;
            case "states": f_conf.emitStates = true; break;

            default:
              let gate = GATES[name];
              if( !gate ){
                error(l, `unknown operation "${name}"`)
              }
              console.log(args, gate.args)
              if( args.length < gate.args.length ){
                error(l, `not enough args to ${name} (need ${gate.args.length})`)
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

        if( !loadingError ){
          for (let i = 0; i < qubits.length; i++) {
            qubits[i].el.remove()
          }
          qubits = []
          for (let i = 0; i < f_conf.qubits; i++) {
            addQubit()
          }

          // TODO: load gates
          gates = []

          conf = f_conf
          $('#check-sparse').prop('checked', conf.isSparse)
          $('#check-group').prop('checked', conf.doGroup)
          $('#slider-shots').val(Math.floor(Math.log2(conf.shots)))
          $('#slider-noise').val(conf.noise * NOISE_SIZE)

          $('.disp-load-name').html(`<code>${file.name}</code>`)
          console.log('loaded successfully!')
          console.log(conf)
          console.log(gates)
          updateDisplay()
          // TODO: actually load
        } else {
          console.log('loaded abysmally!')
        }
        console.log(str)
      })
    })
})
