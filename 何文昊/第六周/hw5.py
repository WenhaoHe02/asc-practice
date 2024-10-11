from fairseq.models import (
    FairseqEncoder,
    FairseqIncrementalDecoder,
    FairseqEncoderDecoderModel,
)
from fairseq.models.transformer import TransformerEncoder, TransformerDecoder
import torch.nn as nn


# 1. Transformer Encoder
class TransformerEncoder(FairseqEncoder):
    def __init__(self, args, dictionary, embed_tokens):
        super().__init__(dictionary)
        self.embed_tokens = embed_tokens
        self.embed_dim = args.encoder_embed_dim

        # Positional Embedding + Transformer Layers
        self.embed_positions = nn.Embedding(1024, self.embed_dim)
        self.layers = nn.ModuleList([
            nn.TransformerEncoderLayer(
                d_model=self.embed_dim,
                nhead=args.encoder_attention_heads,
                dim_feedforward=args.encoder_ffn_embed_dim,
                dropout=args.dropout,
            )
            for _ in range(args.encoder_layers)
        ])
        self.dropout = nn.Dropout(args.dropout)

    def forward(self, src_tokens, src_lengths=None, **unused):
        # Embed tokens and positions
        x = self.embed_tokens(src_tokens) + self.embed_positions(
            torch.arange(0, src_tokens.size(1), device=src_tokens.device).unsqueeze(0)
        )
        x = self.dropout(x)

        # Pass through Transformer layers
        for layer in self.layers:
            x = layer(x)

        return x, None, None  # TransformerEncoder's output

    def reorder_encoder_out(self, encoder_out, new_order):
        return encoder_out[0].index_select(1, new_order), None, None


# 2. Transformer Decoder
class TransformerDecoder(FairseqIncrementalDecoder):
    def __init__(self, args, dictionary, embed_tokens):
        super().__init__(dictionary)
        self.embed_tokens = embed_tokens
        self.embed_dim = args.decoder_embed_dim

        # Positional Embedding + Transformer Layers
        self.embed_positions = nn.Embedding(1024, self.embed_dim)
        self.layers = nn.ModuleList([
            nn.TransformerDecoderLayer(
                d_model=self.embed_dim,
                nhead=args.decoder_attention_heads,
                dim_feedforward=args.decoder_ffn_embed_dim,
                dropout=args.dropout,
            )
            for _ in range(args.decoder_layers)
        ])
        self.dropout = nn.Dropout(args.dropout)

        self.output_projection = nn.Linear(self.embed_dim, len(dictionary))

    def forward(self, prev_output_tokens, encoder_out, incremental_state=None, **unused):
        # Embed tokens and positions
        x = self.embed_tokens(prev_output_tokens) + self.embed_positions(
            torch.arange(0, prev_output_tokens.size(1), device=prev_output_tokens.device).unsqueeze(0)
        )
        x = self.dropout(x)

        # Pass through Transformer decoder layers with cross-attention
        for layer in self.layers:
            x = layer(tgt=x, memory=encoder_out[0])  # Encoder output is the "memory"

        x = self.output_projection(x)
        return x, None

    def reorder_incremental_state(self, incremental_state, new_order):
        return


# 3. Seq2Seq Model with Transformer
class Seq2SeqTransformer(FairseqEncoderDecoderModel):
    def __init__(self, args, encoder, decoder):
        super().__init__(encoder, decoder)
        self.args = args

    def forward(self, src_tokens, src_lengths, prev_output_tokens, return_all_hiddens=True):
        # Encoding
        encoder_out = self.encoder(src_tokens, src_lengths=src_lengths)

        # Decoding
        logits, extra = self.decoder(
            prev_output_tokens,
            encoder_out=encoder_out,
            src_lengths=src_lengths,
            return_all_hiddens=return_all_hiddens,
        )
        return logits, extra


# 4. Build Transformer Model
def build_model(args, task):
    """ 按照参数构建模型 """
    src_dict, tgt_dict = task.source_dictionary, task.target_dictionary

    # Token Embeddings
    encoder_embed_tokens = nn.Embedding(len(src_dict), args.encoder_embed_dim, src_dict.pad())
    decoder_embed_tokens = nn.Embedding(len(tgt_dict), args.decoder_embed_dim, tgt_dict.pad())

    # Encoder and Decoder (Transformer-based)
    encoder = TransformerEncoder(args, src_dict, encoder_embed_tokens)
    decoder = TransformerDecoder(args, tgt_dict, decoder_embed_tokens)

    # Seq2Seq Transformer Model
    model = Seq2SeqTransformer(args, encoder, decoder)

    # Initialize model parameters
    def init_params(module):
        if isinstance(module, nn.Linear):
            module.weight.data.normal_(mean=0.0, std=0.02)
            if module.bias is not None:
                module.bias.data.zero_()
        if isinstance(module, nn.Embedding):
            module.weight.data.normal_(mean=0.0, std=0.02)
            if module.padding_idx is not None:
                module.weight.data[module.padding_idx].zero_()

    model.apply(init_params)
    return model


# 5. Transformer-specific arguments
arch_args = Namespace(
    encoder_embed_dim=256,
    encoder_ffn_embed_dim=1024,
    encoder_layers=4,
    encoder_attention_heads=4,
    decoder_embed_dim=256,
    decoder_ffn_embed_dim=1024,
    decoder_layers=4,
    decoder_attention_heads=4,
    dropout=0.3,
    share_decoder_input_output_embed=True,
)


# Adding necessary Transformer arguments
def add_transformer_args(args):
    args.encoder_attention_heads = 4
    args.decoder_attention_heads = 4
    args.encoder_normalize_before = True
    args.decoder_normalize_before = True
    args.activation_fn = "relu"
    args.max_source_positions = 1024
    args.max_target_positions = 1024


add_transformer_args(arch_args)

model = build_model(arch_args, task)
