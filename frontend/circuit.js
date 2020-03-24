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

  let conf = conf_default();

  const NOISE_SIZE = 10000

  let refreshConf = function(){
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
    $('#btn-qubit-less')[0].disabled = qubits.length <3
  }

  let addQubit = function(){
    let n = qubits.length
    console.log(`qubits +> ${n+1}`)
    let qubit = {
      gates_used: [],
      el: $(`
      <tr class='qubit'>
        <div class='line'></div>
        <td class='header'>q[${n}]</td>
        <td>${ket0}</td>
      </tr>`)
      .appendTo('#qubits')
    }
    qubit.el.children('.header').click(()=>qubit_clicked(n))
    qubits.push(qubit)
    refreshGates()
  };
  $('#btn-qubit-more').click(function(){
    addQubit()
    refreshConf()
  });

  const MATHJAX_URL = "https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"
  let ket0
  $.getScript(MATHJAX_URL, function(){
    setTimeout(function(){
      ket0 = $('.ket0')[0].innerHTML
      addQubit(); addQubit()
      $('html').addClass('mathjax-loaded')
      refreshConf()
      refreshSelector();
    }, 100)
  })
  

  $('#btn-qubit-less').click(function(){
    if (qubits.length > 2) {
      console.log(`qubits -> ${qubits.length - 1}`)
      let line = qubits.pop()
      let lineIsEmpty = true
      if (lineIsEmpty) {
        line.el.remove()
      } else {
        qubits.push(line)
      }
      refreshConf()
    }
  })

  $('#slider-shots').change(refreshConf)
  $('#slider-shots').mousemove(refreshConf)
  $('#slider-noise').change(refreshConf)
  $('#slider-noise').mousemove(refreshConf)

  $('#check-sparse').change(refreshConf)
  $('#check-group').change(refreshConf)

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

  let refreshSelector = function(){
    for (let [i, gate] of Object.entries(GATES)) {
      gate.el.removeClass('selected')
    }
    let msg = "No gate selected."
    let msg_qubit = "Click a gate above to add it to the circuit."
    $('#circuit').removeClass('selection')
    $('#circuit-add-gate').addClass('hidden')

    if( sel.gate ){
      let q = GATES[sel.gate]
      msg = `<strong>${q.el.find('h2').html()}</strong>:`
      q.el.addClass('selected')
      $('#circuit').addClass('selection')

      const selecting_control_qubit = sel.args.length == q.arity
      if( selecting_control_qubit ){
        const can_add_now = q.control_arities.includes(sel.cargs.length)
        if( can_add_now ){
          $('#circuit-add-gate').removeClass('hidden')
        }
        msg_qubit = (can_add_now ? "or s" : "S") + "elect the "
        if ( String(q.control_arities) != "0,1" ){
          msg_qubit += `${ORDS[sel.cargs.length]} `
        }
        msg_qubit += "control qubit."
      } else {
        msg_qubit = "Select the "
        if( q.arity > 1 ){
          msg_qubit += `${ORDS[sel.args.length]} `
        }
        msg_qubit += "qubit to act on."
      }
    }
    $('#status').html(msg)
    $('#status-qubit').html(msg_qubit)
  }

  /* First click selects (or resets) adding a gate. */
  $.each(GATES, function(id, op){
    op.el = $(`#${id}`)
    op.el.click(function(){
      sel.gate = sel.gate == id ? null : id
      refreshSelector()
      op.el.addClass('summon-shushed')
    })
    op.el.mouseout(function(){
      op.el.removeClass('summon-shushed')
    })
  })

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

    /* allow optional extra control qubits */
    if( sel.cargs.length == c[c.length - 1] ){
      tryAddGate()
    } else if( sel.args.length + sel.cargs.length == qubits.length ){
      tryAddGate()
    }
    refreshSelector()
  }

  let tryAddGate = function(){
    /* Try to add the working gate. Will fail gracefully if not complete. */
    if( sel.gate ){
      let q = GATES[sel.gate]
      if (sel.args.length == q.arity && sel.cargs.length in q.control_arities){
        circuit_gates.push(sel)
        sel.all_args = sel.cargs.concat(sel.args)
        for (let i = 0; i < sel.all_args.length; i++) {
          const qindex = sel.all_args[i];
          qubits[qindex].gates_used.push(sel)
        }
        console.log('Gate added:', sel.gate, sel.all_args)
        sel = newSel()
        refreshSelector()
        refreshGates()
      }
    }
  }
  $('#circuit-add-gate button').click(tryAddGate)

  let refreshGates = function(){
    console.log(as_file())
    $('.gate').remove()
    for (let g = 0; g < circuit_gates.length; g++) {
      const gate = circuit_gates[g];
      const top    = Math.min.apply(0, gate.all_args)
      const bottom = Math.max.apply(0, gate.all_args)
      gate.elements = []
      for (let q = 0; q < qubits.length; q++) {
        const qubit = qubits[q];
        let cls = ['gate']
        let content = ""
        if( gate.args.includes(q) ){
          content += gate.gate
        } else if( gate.cargs.includes(q) ){
          content += "o"
        }
        gate.elements.push($(`<td class='${cls.join(' ')}'>${content}</td>`)
          .appendTo(qubit.el))
      }
    }
  }


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
      const id = "c".repeat(gate.cargs.length) + gate.gate
      str += `${id},${gate.all_args.join(",")}\n`
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
          refreshConf()
          refreshSelector()
          refreshGates()
          // TODO: actually load
        } else {
          console.log('loaded abysmally!')
        }
        console.log(str)
      })
    })
})
