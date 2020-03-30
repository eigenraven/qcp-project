let qubits
let circuit_gates

$(function(){

  /* DISPLAY NICETIES */

  /* Ensure right-column panels don't overlap */
  $('.panel-control .summoner').mouseover(function(){
    let anchor = $(this).children('.summon-anchor')
    let anchor_right = $(window).width() - anchor.offset().left
    let right = anchor_right - anchor.width() - 10
    $(this).children('.summoned').css("right", `${right}px`)
  })

  let percent = (x, d) => String(Math.floor(x*100 * 10**d)/10**d)+"%"
  let fadeAt = function(x, limit, min){
    /* If x is above the limit, return 1; if below, fade linearly to the minimum. */
    return x > limit ? 1 : min + (x/limit)*(1-min)
  }

  /* PANEL */
  qubits = Array()

  let conf_default = function(){return {
    noise: 0,
    shots: 1024,
    showBloch: true,
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
    conf.showBloch = ! $('#check-phase').is(':checked')

    $('.disp-qubits').text(qubits.length)
    $('.disp-shots').text(conf.shots)
    let summary = "predicted"
    if (!conf.showBloch){
      let fragments = [
        `${conf.shots} shots`
      ]
      $('.disp-noise-full').text(
        conf.noise ? "a " + percent(conf.noise, 2)
                  : "no") // chance for...
      conf.noise ? fragments.push(percent(conf.noise, 0) +" decay") :0;
      summary = fragments.join(", ")
    }
    $('.disp-summary').text(summary)
    $('#btn-qubit-less').prop("disabled", qubits.length <3)
    refreshGates()
  }

  let addQubit = function(){
    let n = qubits.length
    console.log(`qubits +> ${n+1}`)
    let qubit = {
      gates_used: [],
      el: $(`
      <tr class='qubit'>
        <td class='header'>
          q[${n}]<span class='plus'>+</span>
        </td>
        <td class='initial'>
          \\(\\ket{0}\\)
        </td>
      </tr>`)
      .appendTo('#qubits')
    }
    MathJax.typeset()
    qubit.el.children('.header').click(()=>qubit_clicked(n))
    qubits.push(qubit)
  };
  $('#btn-qubit-more').click(function(){
    addQubit()
    refreshConf()
  });

  const MATHJAX_URL = "https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"
  $.getScript(MATHJAX_URL, function(){
    setTimeout(function(){
      $('html').addClass('mathjax-loaded')
      $('#btn-qubit-less').prop("disabled", false);
      $('#btn-qubit-more').prop("disabled", false);
      addQubit(); addQubit()
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
  $('#check-phase').change(refreshConf)

  /* GATES */
  circuit_gates = []

  let newSel = ()=>Object({
    gate: null,
    args: [],
    cargs: []
  })
  sel = newSel()

  $('#btn-reset').click(function(){
    sel = newSel()
    circuit_gates = [];
    conf = conf_default();
    refreshConf();
    refreshGates();
    refreshSelector();
  })

  const ORDINAL = [
    '1<sup>st</sup>',
    '2<sup>nd</sup>'
  ]

  let refreshSelector = function(){
    for (let [i, gate] of Object.entries(GATES)) {
      gate.el.removeClass('selected')
    }
    let msg = "No gate selected."
    let msg_qubit = "Click one to start."
    $('.builder').removeClass('selection')
    $('#circuit-add-gate').addClass('hidden')

    if( sel.gate ){
      let q = GATES[sel.gate]
      msg = `<strong>${q.el.find('h2').html()}</strong>:`
      q.el.addClass('selected');
      $('.builder').addClass('selection');

      const selecting_control_qubit = sel.args.length == q.arity
      if( selecting_control_qubit ){
        const can_add_now = q.control_arities.includes(sel.cargs.length)
        if( can_add_now ){
          $('#circuit-add-gate').removeClass('hidden')
        }
        msg_qubit = (can_add_now ? "or s" : "S") + "elect the "
        if ( String(q.control_arities) != "0,1" ){
          msg_qubit += `${ORDINAL[sel.cargs.length]} `
        }
        msg_qubit += "control qubit."
      } else {
        msg_qubit = "Select the "
        if( q.arity > 1 ){
          msg_qubit += `${ORDINAL[sel.args.length]} `
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
      if( !tryAddGate() ){
        console.log(`Attempted to add index ${index} (already added)`)
      }
      return;
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
        return true
      }
    }
    return false;
  }
  $('#circuit-add-gate button').click(tryAddGate)

  let refreshGates = function(){    
    $('.gate').remove()
    let column = []
    let colFinished = function(){
      for (let q = 0; q < qubits.length; q++) {
        const ico = column[q] || {
          clsOuter: ["gate"], symbol: "",
          clsInner: ["gate-icon", "empty"]
        }
        $(` <td class="${ico.clsOuter.join(' ')}">
              <div class="${ico.clsInner.join(' ')}">${ico.symbol}</div>
            </td>`).appendTo(qubits[q].el)
      }
      column = []
    }
    for (let g = 0; g < circuit_gates.length; g++) {
      const gate = circuit_gates[g];
      let gateType = GATES[gate.gate]
      const g_min_q = Math.min.apply(0, gate.all_args)
      const g_max_q = Math.max.apply(0, gate.all_args)

      for (let q = g_min_q; q <= g_max_q; q++) {
        if( column[q] ){
          /* would overlap with previous column! */
          colFinished()
          break;
        }
      }
      
      for (let q = g_min_q; q <= g_max_q; q++) {
        let ico = {
          clsOuter: ["gate", gateType.kind],
          clsInner: ["gate-icon"],
          symbol: ""
        }

        if( q != g_min_q ){ico.clsOuter.push('up')}
        if( q != g_max_q ){ico.clsOuter.push('down')}

        if( gate.args.includes(q) ){
          ico.symbol = gateType.kind == "swap"
            ? "×" : `\\(${gateType.symbol}\\)`
        } else if( gate.cargs.includes(q) ){
          ico.clsInner.push("control-dot")
        } else {
          ico.clsInner.push("empty")
        }
        column[q] = ico
      }
    }
    colFinished()
    MathJax.typeset()
    
    $('.state').remove()
    simulate().then(function(states){
      $('.states-error').addClass('hidden')
      $('.states').removeClass('hidden')
      for (let i = 0; i < states.length; i++) {
        const s = states[i];
        let li = percent(s.likelihood, 2)
        let col = fadeAt(s.likelihood, 0.05, 0.3) * 100
        let state = ""
        if (s.real !== undefined) {
          let arg = Math.atan2(s.imag, s.real) * 180 / Math.PI
          state = `
            <td>${s.real < 0 ? "\\(-\\)" : ""}</td>
            <td>\\(${Math.abs(s.real)}\\)</td>
            <td>\\(${s.imag < 0 ? "-" : "+"} ${Math.abs(s.imag)}i\\)</td>
            <td>\\(\\quad(\\phi = ${arg}°)\\)</td>`
        }
        s.el = $(`
          <tr class='state'>
            <td class='state-ket'>
              \\(\\ket{${s.state}}\\)
            </td>
            <td class='bar'>
              <div class='fill'
                  style="width: ${li}; color: rgba(0,0,0,${col}%)">
                ${li}
              </div>
            </td>
            ${state}
          </li>`)
          .appendTo('#states')
      }
      MathJax.typeset()
    }).catch(function(a){
      $('.states-error').removeClass('hidden')
      $('.states').addClass('hidden')
      console.log(a)
    })
  }

  $('#refresh').click(refreshGates)

  /*
   * SIMULATION
   */

  const ENDPOINT = "http://localhost:12345"

  simulate = function(){
    const q = qubits.length
    return new Promise(function(fThen, fError){
      $.post(ENDPOINT + "/simulate", {circuit: as_file()})
      .catch(fError)
      .done(function(data){
        console.log(data)
        const lines = data.split("\n");
        let states = []
        let totalmagnitude = 0;
        let delta = conf.showBloch ? 2 : 1;
        let start = conf.showBloch ? 2+q*2 : 0;
        for (let i = start; i < lines.length; i += delta) {
          let re = lines[i];
          let im, mag;
          if( re && !isNaN(re) ){
            re = Number(re)
            if( conf.showBloch ){
              im = Number(lines[i+1])
              mag = Math.sqrt(re**2 + im**2)
            } else {
              mag = re
              re = undefined;
            }
            let x = states.length
            states.push({
              state: ("0".repeat(q) + x.toString(2)).slice(-q),
              real: re, imag: im, likelihood: mag
            })
            totalmagnitude += mag
          }
        }
        for (let i = 0; i < states.length; i++) {
          states[i].likelihood /= totalmagnitude;
        }
        fThen(states)
      })
    })
  }


  /* 
   * FILE FORMAT AND OPERATIONS 
   */


  as_file = function(){
    str = `qubits,${qubits.length}\n`
    if(conf.showBloch) str += `phase\n`
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

    /* Alas, Safari doesn't support File API */
    let fr = new FileReader()
    fr.readAsText(file)
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

    const FILE_LOAD_TIMEOUT = 200;
    setTimeout(function(){
      lines = fr.result.split(/\r\n|\r|\n/)
      for (let l = 0; l < lines.length; l++) {
        const line = lines[l];
        console.log(line)
        if( !line || line.startsWith(`//`)){
          continue; // strip comments
        }
        let args = line.split(",")
        let name = args.shift().toLowerCase()
        name = GATE_ALIAS[name] || name
        
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
            for (let i = 0; i < args.length; i++) {
              const a = args[i];
              if( isNaN(a) ){
                return error(l, `argument "${a}" not a number`)
              }
              args[i] = Number(a)
            }
            let cargs = []
            while (name.startsWith("c")) {
              cargs.push(args.shift())
              name = name.slice(1)
            }
            let gate = GATES[name];
            if( !gate ){
              return error(l, `unknown operation "${name}"`)
            }
            console.log(name, args, gate)
            if( args.length < gate.arity ){
              return error(l, `not enough args to ${name} (need ${gate.arity})`)
            } else {
              // TODO: assert stuff with indices here
              f_gates.push({
                gate: name,
                args: args,
                cargs: cargs,
                all_args: cargs.concat(args)
              })
            }
            break;
        }
      }
      /* end for loop */
      if( !loadingError ){
        for (let i = 0; i < qubits.length; i++) {
          qubits[i].el.remove()
        }
        qubits = []
        for (let i = 0; i < f_conf.qubits; i++) {
          addQubit()
        }
  
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
    }, FILE_LOAD_TIMEOUT)
  })
})
