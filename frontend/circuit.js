let qubits
let circuit_gates

$(function(){

  /* PANEL */
  qubits = Array()

  let conf_default = function(){return {
    noise: 0,
    shots: 1024,
    isSparse: false,
    doGroup: true,
    emitStates: true,
  }}

  const ket0 = $('.ket0')[0].innerHTML
  let conf = conf_default();

  const NOISE_SIZE = 10000

  let updateDisplay = function(){
    conf.shots = 2 ** $('#slider-shots').val()
    conf.noise = $('#slider-noise').val() / NOISE_SIZE
    conf.isSparse = $('#check-sparse').is(':checked')
    conf.doGroup = $('#check-group').is(':checked')

    $('.disp-qubits').text(qubits.length)
    $('.disp-shots').text(conf.shots)
    let fragments = [
      `${qubits.length} qubits`,
      `${conf.shots} shots`
    ]
    let n = (conf.noise * 100).toLocaleString({maximumFractionDigits: 2})
    $('.disp-noise').text(
      conf.noise ? `${n}%`: "disabled")
    
    conf.noise ? fragments.push(`${n}% decay`) :0;
    conf.isSparse ? fragments.push("sparse") :0;
    conf.doGroup ? fragments.push("grouped") :0;

    $('.disp-summary').text(fragments.join(", "))
  }

  let addQubit = function(){
    let n = qubits.length
    console.log(`qubits +> ${n+1}`)
    qubits.push({
      el: $(`
      <tr class='qubit'>
        <div class='line'></div>
        <td class='header'>q[${n}]</td>
        <td>${ket0}</td>
        <td class='fill'></td>
      </tr>`)
      .appendTo('#qubits')
      .children('.header').click(function(){
        qubit_clicked(n)
      })
    })
  };
  $('#btn-qubit-more').click(function(){
    addQubit()
    updateDisplay()
  });
  
  addQubit(); addQubit(); updateDisplay()

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
  circuit_gates = []

  let newSel = ()=>Object({
    gate: null,
    args: [],
    cargs: []
  })
  sel = newSel()

  const ORDS = [
    '1<sup>st</sup>',
    '2<sup>nd</sup>'
  ]

  let updateGateDisplay = function(){
    for (let [i, gate] of Object.entries(GATES)) {
      gate.el.removeClass('selected')
    }
    let msg = "No gate selected. Click a gate above to add it to the circuit."
    $('#circuit').removeClass('selection')
    $('#circuit-add-gate').addClass('hidden')

    if( sel.gate ){
      let q = GATES[sel.gate]
      q.el.addClass('selected')
      $('#circuit').addClass('selection')
      msg = `Adding <strong>${q.el.find('h2').html()}</strong>. Select `
      if( sel.args.length < q.arity ){
        /* gate qubits */
        if( q.arity > 1 ){
          msg += `${ORDS[sel.args.length]} `
        }
        msg += "qubit to act on."
      } else {
        /* control qubits */
        if( sel.cargs.length in q.control_arities ){
          /* amount of control qubits acceptable, may break out early */
          $('#circuit-add-gate').removeClass('hidden')
          
        }
      }
      
    } else {
    }
    $('#status-1').html(msg)
  }

  /* First click selects (or resets) adding a gate. */
  $.each(GATES, function(id, op){
    op.el = $(`#${id}`)
    op.el.click(function(){
      sel = newSel()
      sel.gate = sel.gate == id ? null : id
      updateGateDisplay()
      op.el.addClass('summon-shushed')
    })
    op.el.mouseout(function(){
      op.el.removeClass('summon-shushed')
    })
  })
  updateGateDisplay();

  /* Selecting a qubit (for now) adds the gate to the end of that qubit. */
  /* This is naturally temporary to bootstrap the rest of functionality. */
  let qubit_clicked = function(index){
    if( !sel.gate ){ return; }
    if( sel.args.includes(index) || sel.cargs.includes(index) ){
      console.log(`Attempted to add index ${index} (already added)`)
      return
    }
    let q = GATES[sel.gate]
    let c = q.control_arities

    if( sel.args.length < q.arity ){
      sel.args.push(index)
    } else {
      sel.cargs.push(index)
    }

    tryAddGate()
    updateGateDisplay()
  }

  let tryAddGate = function(){
    /* Try to add the working gate. Will fail gracefully if not complete. */
    if( sel.gate ){
      let q = GATES[sel.gate]
      if (sel.args.length == q.arity && sel.cargs.length in q.control_arities){
        console.log('Gate added:', sel)
        circuit_gates.push(sel)
        sel = newSel()
        updateGateDisplay()
      }
    }
  }
  $('#circuit-add-gate button').click(tryAddGate)


  /* 
   * FILE FORMAT AND OPERATIONS 
   */


  as_file = function(){
    // TODO: implement form & gathering
    emitStates = true;

    str = `qubits,${qubits.length}\n`
    if(conf.shots != 1024) str += `shots,${conf.shots}\n`
    if(conf.noise) str += `noise,${conf.noise}\n`
    if(conf.isSparse) str += `sparse\n`
    if(!conf.doGroup) str += `nogroup\n`
    if(conf.emitStates) str += `states\n`
    
    for (let i = 0; i < circuit_gates.length; i++) {
      const gate = circuit_gates[i];
      const id = gate.gate
      const args = gate.cargs.concat(gate.args)
      str += `${id},${args.join(",")}\n`
    }
    return str
  }
  let download = function(data, filename, type) {
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

  const GATE_ALIAS = {
    "nop": "id", "hadamard": "h",
    "not": "x",
    "cnot": "cx",
    "tdg": "tinv", "vdg": "vinv",
    "ccnot": "ccx", "toffoli": "ccx", 
  }
  
  let loadingError = false;
  
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
          if( GATE_ALIAS.includes(name) ){
            name = GATE_ALIAS[name]
          }
          
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
          let f_gates = []

          circuit_gates = f_gates
          conf = f_conf
          $('#check-sparse').prop('checked', conf.isSparse)
          $('#check-group').prop('checked', conf.doGroup)
          $('#slider-shots').val(Math.floor(Math.log2(conf.shots)))
          $('#slider-noise').val(conf.noise * NOISE_SIZE)

          $('.disp-load-name').html(`<code>${file.name}</code>`)
          console.log('loaded successfully!')
          console.log(conf)
          console.log(f_gates)
          updateDisplay()
          // TODO: actually load
        } else {
          console.log('loaded abysmally!')
        }
        console.log(str)
      })
    })
})
